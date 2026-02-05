
LOG -s root.clk -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.clk_slow -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.clk_fast -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.clk_fast.frequency_hz -c root.clk_fast -e p -b 0 -d 1 -t txt
LOG -s root.u_symbol_gen.o_symb_ref -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_upsampler.o_signal_i -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_upsampler.o_signal_q -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_parallel_filter.i_signal -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_parallel_filter.o_signal -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_parallel_filter.coeffs -c root.clk_fast -e p -b 0 -d 1 -t txt
LOG -s root.u_ser_to_par.i_serial -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_ser_to_par.r_buffer.i -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_ser_to_par.r_buffer.o -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_par_to_ser.o_serial -c root.clk_fast -e p -b 0 -d 0 -t txt
LOG -s root.u_par_to_ser.i_parallel -c root.clk_fast -e p -b 0 -d 0 -t txt