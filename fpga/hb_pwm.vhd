library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.ALL;

entity hb_pwm is
	Port (
		clk : in STD_LOGIC;
		dutycycle_signed : in  STD_LOGIC_VECTOR (7 downto 0);
		o1 : out  STD_LOGIC;
		o2 : out  STD_LOGIC
	);
end hb_pwm;

architecture Behavioral of hb_pwm is
signal direction : std_logic := 0;
signal prescale_cnt : unsigned(5 downto 0) := 0;
signal counter : unsigned(6 downto 0) := 0;
signal compare : unsigned(6 downto 0) := 0;

--prescaleren skal scale 50 MHz til ca. 20 kHz pwm.
--counteren tæller 127 gange pr. pwm cycle 0->126
--50.000.000 / (20.000 * 127) = 19.7 ~ 20
--faktisk pwm frekvens: 50.000.000 / 20 / 127 = 19685 Hz
constant PRE_CNT_SIZE : unsigned(5 downto 0) := 20;

begin
	
o1 <= direction;
o2 <= '1' when counter < compare else '0';
	
process (clk)
begin
	if rising_edge(clk) then
		if prescale_cnt = PRE_CNT_SIZE - 1 then
			prescale_cnt <= (others => '0');
			if counter = 127 - 1 then
				counter <= (others => '0');
				
				direction <= dutycycle_signed(7);
				if dutycycle_signed(7) = '1' then
					if unsigned(dutycycle_signed(6 downto 0)) = 0 then
						compare <= 0; --: -128 og -127 er begge -100% pwm
					else
						compare <= unsigned(dutycycle_signed(6 downto 0)) - 1;
					end if;
				else
					compare <= unsigned(dutycycle_signed(6 downto 0));
				end if;
			else
				counter <= counter + 1;
			end if;
		else
			prescale_cnt <= prescale_cnt + 1;
		end if;
	end if;
end process;

end Behavioral;