
LOG -s root.clk -c root.clk -e p -b 0 -d 0 -t txt
LOG -s root.clk.frequency_hz -c root.clk -e p -b 0 -d 1 -t txt
LOG -s root.u_sin_gen.o_sin -c root.clk -e p -b 0 -d 0 -t txt
LOG -s root.u_filter.o_signal -c root.clk -e p -b 0 -d 0 -t txt
LOG -s root.u_filter.coeffs -c root.clk -e p -b 0 -d 1 -t txt