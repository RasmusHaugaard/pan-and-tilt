library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.ALL;

entity hbridge_pwm is
	Port (
		clk : in STD_LOGIC;
		signed_dutycycle : in  STD_LOGIC_VECTOR (7 downto 0);
      o1 : out  STD_LOGIC;
      o2 : out  STD_LOGIC
	);
end hbridge_pwm;

architecture Behavioral of hbridge_pwm is
signal prescaler : unsigned(5 downto 0);
signal counter : unsigned(6 downto 0);
signal compare : unsigned(6 downto 0);
begin

o1 <= signed_dutycycle(7);
compare <= unsigned(signed_dutycycle(6 downto 0));

--prescaleren skal scale 50 MHz til ca. 20 kHz pwm.
--counteren t�ller 128 gange pr. pwm cycle.
--50.000.000 / (20.000 * 128) = 19.5 ~ 20
--faktisk pwm frekvens: 50.000.000 / 20 / 128 = 19531.25 Hz
PRESCALE: process (clk)
begin
	if rising_edge(clk) then
		if prescaler = 19 then
			counter <= counter + 1;
			prescaler <= (others => '0');
		else
			prescaler <= prescaler + 1;
		end if;
	end if;
end process;

PWM: process (compare, counter)
begin
	if counter < compare then
		o2 <= '1';
	else
		o2 <= '0';
	end if;
end process;

end Behavioral;