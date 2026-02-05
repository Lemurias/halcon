
LOG -s root.clk_ch -c root.clk_ch -e p -b 0 -p 1 -d 0 -t b -n l -f 0
LOG -s root.clk_br_tx -c root.clk_br_tx -e p -b 0 -p 1 -d 0 -t b -n l -f 0
LOG -s root.u_symbol_gen.o_symb_tx -c root.clk_br_tx -e p -b 0 -p 1 -d 0 -t b -n l -f 0
LOG -s root.u_symbol_gen.o_symb_ref -c root.clk_br_tx -e p -b 0 -p 1 -d 0 -t b -n l -f 0
LOG -s root.clk_br_rx -c root.clk_br_rx -e p -b 0 -p 1 -d 0 -t b -n l -f 0
LOG -s root.u_downsampler.o_signal -c root.clk_br_rx -e p -b 0 -p 1 -d 0 -t b -n l -f 0
LOG -s root.u_slicer.o_signal -c root.clk_br_rx -e p -b 0 -p 1 -d 0 -t b -n l -f 0
LOG -s root.u_slicer.i_signal -c root.clk_br_rx -e p -b 0 -p 1 -d 0 -t b -n l -f 0
LOG -s root.u_slicer.o_error -c root.clk_br_rx -e p -b 0 -p 1 -d 0 -t b -n l -f 0
SET -v "1" -s root.u_ber_counter.enable -c root.clk_br_rx -e p -b 0 -d 0