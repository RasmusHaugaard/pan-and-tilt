library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity spi_module is
	Port (
		clk : in STD_LOGIC;
		spi_ss, spi_clk, spi_mosi : in  STD_LOGIC;
		spi_miso : out STD_LOGIC;
		enc_tilt_A, enc_tilt_B: in  STD_LOGIC;
		pwm_tilt_o1, pwm_tilt_o2 : out  STD_LOGIC;
		
		--DEBUG
		led  : out STD_LOGIC_VECTOR (7 downto 0)
	);

end spi_module;

architecture Behavioral of spi_module is
--CONSTANTS
	constant dummy : std_logic_vector(7 downto 0) := "00000000";
	--constant pwm_pan_addr : std_logic_vector(5 downto 0) := "000001";
	constant hb_pwm_tilt_addr : std_logic_vector(5 downto 0) := "000010";
	--constant encoder_pan_addr : std_logic_vector(5 downto 0) := "000011";
	constant enc_tilt_val_addr : std_logic_vector(5 downto 0) := "000100";
	--constant homing_pan_addr : std_logic_vector(5 downto 0) := "000101";
	constant homing_tilt_addr : std_logic_vector(5 downto 0) := "000110";
	constant spi_state_read : std_logic := '1';
	constant spi_state_write : std_logic := '0';
--SIGNALS
	--SPI
	signal spi_shift_in, spi_shift_out : std_logic_vector(7 downto 0) := dummy;
	signal spi_clk_d, spi_ss_d : std_logic;
	signal spi_state : std_logic := spi_state_read;
	signal spi_address : std_logic_vector (5 downto 0);
	signal pulse_1MHz : STD_LOGIC;
	--ENC
	signal enc_tilt_val : std_logic_vector(7 downto 0):= "00000000";
	--PWM
	signal hb_pwm_tilt_val : std_logic_vector(7 downto 0):= "00000000";
begin 
	prescaler_1Mhz_ent : entity work.prescaler generic map(prescale_val => 50) port map(
		clk => clk,
		clk_scaled => pulse_1MHz
	);
	hb_pwm_tilt: entity work.hb_pwm PORT MAP(
		clk => clk,
		dutycycle_signed => hb_pwm_tilt_val,
		o1 => pwm_tilt_o1,
		o2 => pwm_tilt_o2
	);
	enc_tilt_ent: entity work.encoder PORT MAP(
		clk => clk,
		Ain => enc_tilt_A,
		Bin => enc_tilt_B,
		val => enc_tilt_val
	);
	
	process (clk)
	begin
		if rising_edge(clk) then
			if pulse_1MHz='1' then
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
										when
											enc_tilt_val_addr => 
												spi_shift_out <= enc_tilt_val;
										when others => spi_shift_out <= dummy;
									end case;
								end if;
							end if;
						else -- spi_state = spi_state_write
							case spi_address is
								when hb_pwm_tilt_addr => hb_pwm_tilt_val <= spi_shift_in;
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