library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity prescaler is
	Generic (
		prescale_val : integer -- min 2
	);
	Port ( 
		clk : in  STD_LOGIC;
		clk_scaled : out  STD_LOGIC
	);
end prescaler;

architecture Behavioral of prescaler is
	signal counter : integer := prescale_val;
begin

process (clk)
begin
	if rising_edge(clk) then
		if counter = 1 then
			clk_scaled <= '1';
			counter <= prescale_val;
		else
			clk_scaled <= '0';
			counter <= counter - 1;
		end if;
	end if;
end process;

end Behavioral;

