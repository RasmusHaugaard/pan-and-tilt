library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity spi_module is
	Port (
		clk : in STD_LOGIC;
		spi_ss_in, spi_clk_in, spi_mosi : in  STD_LOGIC;
		spi_miso : out STD_LOGIC;
		homing_index_0, homing_index_1: in STD_LOGIC;
		enc_1_a, enc_1_b: in STD_LOGIC;
		enc_2_a, enc_2_b: in STD_LOGIC;
		mot_a_o1, mot_a_o2, mot_a_en : out  STD_LOGIC;
		mot_b_o1, mot_b_o2, mot_b_en : out  STD_LOGIC;
		--DEBUG
		led  : out STD_LOGIC_VECTOR (7 downto 0)
	);

end spi_module;

architecture Behavioral of spi_module is
--CONSTANTS
	constant dummy : std_logic_vector(7 downto 0) := "00000000";
	constant hb_pwm_pan_addr : std_logic_vector(5 downto 0) := "000001";
	constant hb_pwm_tilt_addr : std_logic_vector(5 downto 0) := "000010";
	constant enc_pan_val_addr : std_logic_vector(5 downto 0) := "000011";
	constant enc_tilt_val_addr : std_logic_vector(5 downto 0) := "000100";
	constant homing_pan_addr : std_logic_vector(5 downto 0) := "000101";
	constant homing_tilt_addr : std_logic_vector(5 downto 0) := "000110";
	constant spi_state_read : std_logic := '1';
	constant spi_state_write : std_logic := '0';
--SIGNALS
	--SPI
	signal spi_shift_in, spi_shift_out : std_logic_vector(7 downto 0) := dummy;
	signal spi_clk_shift, spi_ss_shift : std_logic_vector(9 downto 0);
	signal spi_clk_d, spi_ss_d : std_logic;
	signal spi_clk : std_logic := '0';
	signal spi_ss : std_logic := '1';
	signal spi_state : std_logic := spi_state_read;
	signal spi_address : std_logic_vector (5 downto 0);
	signal spi_pulse : STD_LOGIC;
	--ENC
	signal enc_tilt_val : std_logic_vector(7 downto 0):= "00000000";
	signal enc_pan_val : std_logic_vector(7 downto 0):= "00000000";
	--PWM
	signal hb_pwm_tilt_val : std_logic_vector(7 downto 0):= "00000000";
	signal hb_pwm_pan_val : std_logic_vector(7 downto 0):= "00000000";
begin 

	mot_a_en <= '1';
	mot_b_en <= '1';
	
	prescaler_1Mhz_ent : entity work.prescaler generic map(prescale_val => 5) port map(
		clk => clk,
		clk_scaled => spi_pulse
	);
	hb_pwm_tilt: entity work.hb_pwm PORT MAP(
		clk => clk,
		dutycycle_signed => hb_pwm_tilt_val,
		o1 => mot_a_o1,
		o2 => mot_a_o2
	);
	hb_pwm_pan: entity work.hb_pwm PORT MAP(
		clk => clk,
		dutycycle_signed => hb_pwm_pan_val,
		o1 => mot_b_o1,
		o2 => mot_b_o2
	);
	enc_tilt_ent: entity work.encoder PORT MAP(
		clk => clk,
		Ain => enc_1_a,
		Bin => enc_1_b,
		val => enc_tilt_val
	);
	enc_pan_ent: entity work.encoder PORT MAP(
		clk => clk,
		Ain => enc_2_a,
		Bin => enc_2_b,
		val => enc_pan_val
	);
	
	process (clk)
	begin
		if rising_edge(clk) then
			if spi_pulse='1' then
			
				spi_clk_shift <= spi_clk_shift(8 downto 0)& spi_clk_in;
				spi_ss_shift <= spi_ss_shift(8 downto 0)& spi_ss_in;
				
				if spi_clk_shift = "1111111111" then
					spi_clk <= '1';
				elsif spi_clk_shift = "0000000000" then
					spi_clk <= '0';
				end if;
				
				if spi_ss_shift = "1111111111" then
					spi_ss <= '1';
				elsif spi_ss_shift = "0000000000" then
					spi_ss <= '0';
				end if;
			
				spi_clk_d  <= spi_clk;
				spi_ss_d  <= spi_ss;
					-- if slave becomes selected 1 -> 0
					if spi_ss_d = '1' and spi_ss = '0' then
						spi_miso <= spi_shift_out(7);
						spi_shift_out(7 downto 0) <= spi_shift_out(6 downto 0) & '0';
					end if;
					-- if slave is selected
					if spi_ss = '0' then
						if spi_clk_d = '0' and spi_clk = '1' then
							spi_shift_in <= spi_shift_in(6 downto 0) & spi_mosi;
						end if;
						if spi_clk_d = '1' and spi_clk = '0' then
							spi_miso <= spi_shift_out(7);
							spi_shift_out(7 downto 0) <= spi_shift_out(6 downto 0) & '0';
						end if;
					end if;
					-- if slave becomes unselected 0 -> 1
					if spi_ss_d = '0' and spi_ss = '1' then
						if spi_state = spi_state_read then
							if spi_shift_in /= dummy then -- if received byte != dummy byte
								if spi_shift_in(7) = '0' then -- request to write to address received
									spi_state <= spi_state_write;
									spi_address <= spi_shift_in(5 downto 0);
								else -- request to read from address received
									spi_state <= spi_state_read;									
									case spi_shift_in(5 downto 0) is																		 
										when enc_tilt_val_addr => spi_shift_out <= enc_tilt_val;
										when enc_pan_val_addr => spi_shift_out <= enc_pan_val;
										when homing_tilt_addr => spi_shift_out <= "0000000" & homing_index_0;
										when homing_pan_addr => spi_shift_out <= "0000000" & homing_index_1;
										when others => spi_shift_out <= dummy;
									end case;
								end if;
							end if;
						else -- spi_state = spi_state_write
							case spi_address is
								when hb_pwm_tilt_addr => hb_pwm_tilt_val <= spi_shift_in;
								when hb_pwm_pan_addr => hb_pwm_pan_val <= spi_shift_in;
								when others => NULL;
							end case;
							spi_shift_out <= dummy;
							spi_state <= '1';
						end if;					
					end if;
				end if; --spi pulse
        end if; --rising edge clk
    end process;
	 
	 led <= enc_tilt_val;
end Behavioral;