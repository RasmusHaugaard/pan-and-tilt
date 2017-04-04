library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.all;

entity encoder is
	Port ( 
		clk : in  STD_LOGIC;
		res : in  STD_LOGIC;
		A : in  STD_LOGIC;
		B : in  STD_LOGIC;
		val : out STD_LOGIC_VECTOR(7 downto 0)
	);
end encoder;

architecture Behavioral of encoder is
signal LA : STD_LOGIC;
signal LB : STD_LOGIC;
signal cnt : signed(7 downto 0);
begin

process (clk)
begin
	if rising_edge(clk) then
	
		if res = '1' then
			cnt <= (others => '0');
		else
			if LA = '0' and A = '1' then
				if B = '1' then
					cnt <= cnt + 1;
				else
					cnt <= cnt - 1;
				end if;
			elsif LA = '1' and A = '0' then
				if B = '1' then
					cnt <= cnt - 1;
				else
					cnt <= cnt + 1;
				end if;
			end if;
			if LB = '0' and B = '1' then
				if A = '1' then
					cnt <= cnt - 1;
				else
					cnt <= cnt + 1;
				end if;
			elsif LB = '1' and B = '0' then
				if A = '1' then
					cnt <= cnt + 1;
				else
					cnt <= cnt - 1;
				end if;
			end if;
			
			LA <= A;
			LB <= B;
		end if;
	end if;
end process;

val <= STD_LOGIC_VECTOR(cnt);

end Behavioral;

