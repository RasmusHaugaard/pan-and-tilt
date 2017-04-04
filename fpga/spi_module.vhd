library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity spi_module is
    Port ( spi_ss : in  STD_LOGIC;
           spi_clk : in  STD_LOGIC;
           spi_data_in : in  STD_LOGIC;
           spi_data_out : out  STD_LOGIC;
           led  : out STD_LOGIC_VECTOR (7 downto 0);
			  o1_pan : out  STD_LOGIC;
			  o2_pan : out  STD_LOGIC;
			  o1_tilt : out  STD_LOGIC;
			  o2_tilt : out  STD_LOGIC;
			  A : in  STD_LOGIC;
		     B : in  STD_LOGIC;
			  fpgaclk : in STD_LOGIC);

end spi_module;

architecture Behavioral of spi_module is

	COMPONENT hbridge_pwm
	PORT(
		clk : IN std_logic;
		signed_dutycycle : IN std_logic_vector(7 downto 0);          
		o1 : OUT std_logic;
		o2 : OUT std_logic
		);
	END COMPONENT;
	
	COMPONENT encoder
	PORT(
		clk : IN std_logic;
		res : IN std_logic;
		A : IN std_logic;
		B : IN std_logic;          
		Led : OUT std_logic_vector(7 downto 0)
		);
	END COMPONENT;

    signal shiftreg_in : std_logic_vector(7 downto 0) := "00000000";
	 signal shiftreg_out : std_logic_vector(7 downto 0) := "00000000";
    signal sck_s    : std_logic;  -- sync incoming sck to FPGA clock
    signal sck_d    : std_logic;  -- delay flop for edge detect
    signal mosi_s   : std_logic;  -- sync to FPGA clock
    signal nss_s    : std_logic;  -- sync
    signal nss_d    : std_logic;  -- delay for edge detect
	 signal spi_state : std_logic := '1'; -- R/W' mode. Default: read.
	 signal dummy : std_logic_vector(7 downto 0) := "00000000";
	 signal spi_address : std_logic_vector (5 downto 0);
	 signal reset : std_logic := '0';	 
	 signal PWM_pan : std_logic_vector(7 downto 0):= "00000000";
	 signal PWM_tilt : std_logic_vector(7 downto 0):= "00000000";
	 signal encoder_pan : std_logic_vector(7 downto 0);
	 signal encoder_tilt : std_logic_vector(7 downto 0);
	 signal homing_pan : std_logic_vector(7 downto 0);
	 signal homing_tilt : std_logic_vector(7 downto 0);
	 
	 constant PWM_pan_addr : std_logic_vector(5 downto 0) := "000001";
	 constant PWM_tilt_addr : std_logic_vector(5 downto 0) := "000010";
	 constant encoder_pan_addr : std_logic_vector(5 downto 0) := "000011";
	 constant encoder_tilt_addr : std_logic_vector(5 downto 0) := "000100";
	 constant homing_pan_addr : std_logic_vector(5 downto 0) := "000101";
	 constant homing_tilt_addr : std_logic_vector(5 downto 0) := "000110";
	 
begin 

	hbridge_pwm_pan: hbridge_pwm PORT MAP(
		clk => fpgaclk,
		signed_dutycycle => PWM_pan,
		o1 => o1_pan,
		o2 => o2_pan
		);
		
	hbridge_pwm_tilt: hbridge_pwm PORT MAP(
		clk => fpgaclk,
		signed_dutycycle => PWM_tilt,
		o1 => o1_tilt,
		o2 => o2_tilt
		);
		
		
	Inst_encoder_pan: encoder PORT MAP(
		clk => fpgaclk,
		res => reset,
		A => A,
		B => B,
		Led => encoder_tilt
	);
		
		
    spi : process (fpgaclk) is
    begin
        if rising_edge(fpgaclk) then
            -- synchronization:
            sck_s  <= spi_clk;
            mosi_s <= spi_data_in;
            nss_s  <= spi_ss;
            -- delay for edge detect:
            sck_d  <= sck_s;
            nss_d  <= nss_s;
				
				reset <= '0';
				led <= PWM_tilt;
				--encoder_pan <= "0000"&sw;
 
            -- shifter.
            slaveselected : if nss_s = '0' then
                sck_rising_edge : if sck_s = '1' and sck_d = '0' then
                    shiftreg_in <= shiftreg_in(shiftreg_in'left -1 downto 0) & mosi_s;
                end if sck_rising_edge;
					 
					 sck_falling_edge : if sck_s = '0' and sck_d = '1' then
							spi_data_out <= shiftreg_out(7);
							shiftreg_out(7 downto 1) <= shiftreg_out(6 downto 0);
                end if sck_falling_edge;
            end if slaveselected;
 
            -- save incoming word.
            if nss_s = '1' and nss_d = '0' then
					if spi_state = '1' then
					
						if shiftreg_in /= dummy then
							if shiftreg_in(7) = '0' then
								spi_state <= '0';
								spi_address <= shiftreg_in(5 downto 0);
							else
								spi_state <= '1';
								spi_address <= shiftreg_in(5 downto 0);
								case shiftreg_in(5 downto 0) is
									when encoder_pan_addr => shiftreg_out <= encoder_pan;
																	 reset <= '1';
																	 
									when encoder_tilt_addr => shiftreg_out <= encoder_tilt;
																	  reset <= '1';
									when homing_pan_addr => shiftreg_out <= homing_pan;
									when homing_tilt_addr => shiftreg_out <= homing_tilt;
									when others => shiftreg_out <= dummy;
								end case;
								
							end if;
						end if;
					else			
						case spi_address is
							when PWM_pan_addr => PWM_pan <= shiftreg_in;
							when PWM_tilt_addr => PWM_tilt <= shiftreg_in;
							when others => shiftreg_out <= dummy;
						end case;
						spi_state <= '1';
					end if;					
            end if;
        end if; 
    end process spi;
end Behavioral;