module FreqCounter(rst_n, clk, refer, ready, sck, cs, miso);

input rst_n, clk, refer, sck, cs;
output miso;
output ready;

wire [31:0] sum;
wire [31:0] latchedSum;

Counter counter(rst_n, clk, refer, sum);
D_FF dff(rst_n, refer, cs, sum, latchedSum);
SpiSlave spi(rst_n, latchedSum, sck, cs, miso);

assign ready = refer;

endmodule

// Counter
module Counter(rst_n, clk, clr, sum);

input rst_n, clk, clr;
output reg [31:0] sum;
reg lastClr;

always @(negedge rst_n or posedge clk)
begin
	if (~rst_n)
	begin
		sum <= 32'b0;
		lastClr <= 1'b0;
	end	
	else
		if (clr && lastClr == ~clr)
		begin // posedge clr
			sum <= 32'b0;
			lastClr <= clr;
		end
		else
		begin
			sum <= sum + 32'b1;
			lastClr <= clr;
		end
end

endmodule

// DFF
module D_FF(rst_n, trig, en, d, q);

input rst_n, trig, en;
input [31:0] d;
output reg [31:0] q;

always @(negedge rst_n or posedge trig)
begin
	if (~rst_n)
		q <= 32'b0;
	else
	begin
		if (en)
			q <= d;
		else
			q <= q;
	end
end

endmodule

// SPI Slave
// MSB first, CPOL = 1, CPHA = 1
module SpiSlave(rst_n, sum, sck, cs, miso);

input rst_n, sck, cs;
input [31:0] sum;
output reg miso;

reg [4:0] i;

always @(negedge rst_n or posedge cs or negedge sck)
begin
	if (~rst_n)
	begin
		miso <= 1'b0;
		i <= 5'b11111;
	end
	else if (cs)
	begin
		miso <= 1'b0;
		i <= 5'b11111;
	end
	else // if (~cs)
	begin
		miso <= sum[i];
		i <= i - 5'b1;
	end
end

endmodule
