library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.all;

entity encoder is
	Port ( 
		clk : in  STD_LOGIC;
		Ain, Bin : in  STD_LOGIC;
		val : out STD_LOGIC_VECTOR(7 downto 0)
	);
end encoder;

architecture Behavioral of encoder is
signal A, B, LA, LB : STD_LOGIC;
signal AS, BS : STD_LOGIC_VECTOR(9 downto 0);
signal cnt : signed(7 downto 0);
signal pulse_60KHz : STD_LOGIC;
begin

pulse_ent : entity work.prescaler generic map(prescale_val => 833) port map( --50.000.000 / 833 = 60.000
	clk => clk,
	clk_scaled => pulse_60KHz
);

process (clk)
begin
	if rising_edge(clk) then
		if pulse_60KHz = '1' then
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
			LA <= A;
			LB <= B;

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
			elsif LB = '0' and B = '1' then
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
		end if;
	end if;
end process;

val <= STD_LOGIC_VECTOR(cnt);

end Behavioral;

