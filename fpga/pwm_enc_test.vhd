library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity pwm_enc_test is
	Port (
		led  : out STD_LOGIC_VECTOR (7 downto 0);
		o1_tilt : out  STD_LOGIC;
		o2_tilt : out  STD_LOGIC;
		A : in  STD_LOGIC;
		B : in  STD_LOGIC;
		fpgaclk : in STD_LOGIC;
		Sw : in std_logic_vector(7 downto 0)
	);

end pwm_enc_test;

architecture Behavioral of pwm_enc_test is
	signal PWM_tilt : std_logic_vector(7 downto 0):= "00100000";
	signal reset : std_logic := '0';
	signal encoder_tilt : std_logic_vector(7 downto 0);
begin
	pwm_tilt_ent: entity work.hbridge_pwm PORT MAP(
		clk => fpgaclk,
		signed_dutycycle => PWM_tilt,
		o1 => o1_tilt,
		o2 => o2_tilt
	);
	encoder_tilt_ent: entity work.encoder PORT MAP(
		clk => fpgaclk,
		res => reset,
		A => A,
		B => B,
		val => encoder_tilt
	);
	
led <= encoder_tilt;
Pwm_tilt <= Sw;

end Behavioral;

