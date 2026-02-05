
LOG -s root.clk -c root.clk -e p -b 0 -d 0 -t bin
LOG -s root.clk.frequency_hz -c root.clk -e p -b 0 -d 1 -t bin
LOG -s root.u_symbol_gen.o_symb_ref -c root.clk -e p -b 0 -d 0 -t bin
LOG -s root.u_filter_iir.i_signal -c root.clk -e p -b 0 -d 0 -t bin
LOG -s root.u_filter_iir.o_signal -c root.clk -e p -b 0 -d 0 -t bin
LOG -s root.u_filter_iir.alpha -c root.clk -e p -b 0 -d 1 -t bin