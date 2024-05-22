package tta0_globals is
  -- instruction width
  constant INSTRUCTIONWIDTH : positive := 172;
  -- address width of the instruction memory
  constant IMEMADDRWIDTH : positive := 21;
  -- width of the instruction memory in MAUs
  constant IMEMWIDTHINMAUS : positive := 1;
  -- clock period
  constant PERIOD : time := 10 ns;
end tta0_globals;
