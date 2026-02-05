
LOG -s root.clk -c root.clk_oversampling -e p -b 0 -d 0 -t txt
LOG -s root.clk.frequency_hz -c root.clk_oversampling -e p -b 0 -d 1 -t txt
LOG -s root.clk_oversampling -c root.clk_oversampling -e p -b 0 -d 0 -t txt
LOG -s root.clk_oversampling.frequency_hz -c root.clk_oversampling -e p -b 0 -d 1 -t txt
LOG -s root.u_upsampling.i_signal -c root.clk -e p -b 0 -d 0 -t txt
LOG -s root.u_upsampling.o_signal -c root.clk_oversampling -e p -b 0 -d 0 -t txt
LOG -s root.u_rrc_filter.o_signal -c root.clk_oversampling -e p -b 0 -d 0 -t txt
LOG -s root.u_rrc_filter.o_shift_reg -c root.clk_oversampling -e p -b 0 -d 0 -t txt
LOG -s root.u_rrc_filter.coeffs -c root.clk_oversampling -e p -b 0 -d 1 -t txt