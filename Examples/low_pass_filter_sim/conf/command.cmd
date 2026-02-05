
LOG -s root.clk -c root.clk -e p -b 0 -p 1 -d 0 -t t -n l -f 0
LOG -s root.u_adder.o_sum -c root.clk -e p -b 0 -p 1 -d 0 -t t -n l -f 0
LOG -s root.u_filter.o_signal -c root.clk -e p -b 0 -p 1 -d 0 -t t -n l -f 0
LOG -s root.u_filter.o_shift_reg -c root.clk -e p -b 0 -p 1 -d 0 -t t -n l -f 0
LOG -s root.u_sin_low.o_sin -c root.clk -e p -b 0 -p 1 -d 0 -t t -n l -f 0
LOG -s root.u_sin_high.o_sin -c root.clk -e p -b 0 -p 1 -d 0 -t t -n l -f 0
LOG -s root.clk.i_frequency_hz -c root.clk -e p -b 0 -p 1 -d 1 -t t -n l -f 0
LOG -s root.u_filter.coeffs -c root.clk -e p -b 0 -p 1 -d 1 -t t -n l -f 0