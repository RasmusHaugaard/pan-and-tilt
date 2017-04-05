library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.all;

entity encoder is
	Port ( 
		clk : in  STD_LOGIC;
		res : in  STD_LOGIC;
		Ain : in  STD_LOGIC;
		Bin : in  STD_LOGIC;
		val : out STD_LOGIC_VECTOR(7 downto 0)
	);
end encoder;

architecture Behavioral of encoder is
signal LA : STD_LOGIC;
signal LB : STD_LOGIC;
signal A : STD_LOGIC;
signal B : STD_LOGIC;
signal AS : STD_LOGIC_VECTOR(9 downto 0);
signal BS : STD_LOGIC_VECTOR(9 downto 0);
signal cnt : signed(7 downto 0);
signal clk_scaled : STD_LOGIC;
constant prescale_val : integer := 833;
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


process (clk)
begin
	if rising_edge(clk) then
		AS <= AS(8 downto 0)& Ain;
		BS <= BS(8 downto 0)& Bin;
		
		if AS = "1111111111" then
			A <= '1';
		elsif AS = "0000000000" then
			A <= '0';
		end if;

		if BS = "1111111111" then
			B <= '1';
		elsif BS = "0000000000" then
			B <= '0';
		end if;
	
		if res = '1' then
			cnt <= (others => '0');
		elsif clk_scaled = '1' then
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

