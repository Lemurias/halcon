
LOG -s root.clk -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_transmitter.u_symbol_gen.o_symb_tx -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_transmitter.u_symbol_gen.o_symb_ref -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_channel.u_awgn_ch.o_signal -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_receiver.u_slicer.i_signal -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_receiver.u_slicer.o_signal -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_receiver.u_slicer.o_error -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_ber_counter.enable -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_ber_counter.phase -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
LOG -s root.u_ber_counter.o_ber_value -c root.clk -e p -b 0 -p 1 -d 100000 -t t -n l -f 0
SET -v "1" -s root.u_ber_counter.enable -c root.clk -e p -b 0 -d 0