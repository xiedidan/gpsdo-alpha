module LcdBridge(rst_n, clk, rs, rw, e, d, cs, sck, mosi, miso);

input rst_n, clk, cs, sck, mosi;
output rs, rw, e, miso;

// d is bidirectional port
inout [7:0] d;
wire [7:0] outData;
wire dFlag; // if dFlag == 1, d is output
assign d = dFlag ? outData : 8'bzzzz_zzzz;

wire [7:0] ctl, data, ret_data;
wire divClk;

ClockGenerator gen(rst_n, clk, divClk);
LcdController lcd(rst_n, divClk, cs, ctl, dFlag, d, outData, rs, rw, e, data, ret_data);
SpiSlave spi(rst_n, cs, sck, mosi, miso, ctl, data, ret_data);

endmodule

// divide clk
module ClockGenerator(rst_n, clk, divClk);

input rst_n, clk;
output reg divClk;

parameter clkDiv = 8'b0001_1111; // clkDiv = 32, 64fs
reg [7:0] clkCount;

always @(posedge clk)
begin
	if (~rst_n)
	begin
		divClk <= 1'b0;
		clkCount <= 8'b0;
	end
	else if (clkCount == clkDiv)
	begin
		divClk <= ~divClk;
		clkCount <= 8'b0;
	end
	else
		clkCount <= clkCount + 8'b1;
end

endmodule

// LCD Controller
module LcdController(rst_n, clk, trigger, ctl, dFlag, in_d, out_d, rs, rw, e, data, ret_data);

input rst_n, clk, trigger;
input [7:0] ctl, data;
output reg rs, rw, e;
output reg [7:0] ret_data;

// bidirectional port
output reg dFlag;
input [7:0] in_d;
output reg [7:0] out_d;

reg [4:0] tsp1;
parameter minTsp1 = 30;
reg [7:0] tpw;
parameter minTpw = 150;
reg [3:0] thd;
parameter minThd = 20;
reg [3:0] trest;
parameter minTrest = 250;

// FSM
// general
parameter sIdle = 4'b0000;
// write
parameter sWriteSetup = 4'b0010;
parameter sWriteEnable = 4'b0011;
parameter sWriteDisable = 4'b0100;
parameter sWriteRelease = 4'b0101;
// read
parameter sReadSetup = 4'b0110;
parameter sReadEnable = 4'b0111;
parameter sReadDisable = 4'b1000;
parameter sReadRelease = 4'b1001;
// states
reg [3:0] nextState;
reg [3:0] prevState;

// output setup for states
always @(posedge clk)
begin
	if (rst_n)
	begin
		case (prevState)
			sIdle:
			begin
				rs <= 1'b0; // cmd
				rw <= 1'b1; // write
				e <= 1'b0; // disable
				dFlag <= 1'b1; // output d
				out_d <= 8'b0000_0000;
			end
			
			sWriteSetup:
			begin
				rs <= ctl[1];
				rw <= ctl[0]; // this should be 1 here
				e <= 1'b0;
				dFlag <= 1'b1; // output d
				out_d <= data; // write to d
			end
			
			sWriteEnable:
			begin
				rs <= ctl[1];
				rw <= ctl[0]; // this should be 1 here
				e <= 1'b1; // enable
				dFlag <= 1'b1; // output d
				out_d <= data; // write to d
			end
			
			sWriteDisable:
			begin
				rs <= ctl[1];
				rw <= ctl[0]; // this should be 1 here
				e <= 1'b0; // disable
				dFlag <= 1'b1; // output d
				out_d <= data; // write to d
			end
			
			sWriteRelease:
			begin
				rs <= 1'b0; // cmd
				rw <= 1'b1; // write
				e <= 1'b0;
				dFlag <= 1'b1; // output d
				out_d <= 8'b0000_0000;
			end
			
			sReadSetup:
			begin
				rs <= ctl[1];
				rw <= ctl[0]; // should be 0 here
				e <= 1'b0;
				dFlag <= 1'b0; // input d
				out_d <= 8'b0000_0000;
			end
			
			sReadEnable:
			begin
				rs <= ctl[1];
				rw <= ctl[0]; // should be 0 here
				e <= 1'b1;
				dFlag <= 1'b0; // input d
				out_d <= 8'b0000_0000;
			end
			
			sReadDisable:
			begin
				rs <= ctl[1];
				rw <= ctl[0]; // should be 0 here
				e <= 1'b0;
				ret_data <= in_d; // read from d, TO DO: switch to dff
				dFlag <= 1'b0; // input d
				out_d <= 8'b0000_0000;
			end
			
			sReadRelease:
			begin
				rs <= 1'b0; // cmd
				rw <= 1'b1; // write
				e <= 1'b0;
				dFlag <= 1'b1; // output d
				out_d <= 8'b0000_0000;
			end
		endcase
	end
end

// FSM
always @(posedge clk)
if (rst_n)
begin
	case (prevState)
	sIdle:
	begin
		if (trigger)
			if (ctl[0])
				nextState <= sWriteSetup;
			else
				nextState <= sReadSetup;
		else
			nextState <= sIdle;
	end
	
	sWriteSetup:
	begin
		if (tsp1 == 5'b0)
		begin
			tsp1 <= minTsp1;
			nextState <= sWriteEnable;
		end
		else
		begin
			tsp1 <= tsp1 - 5'b1;
			nextState <= sWriteSetup;
		end
	end
	
	sWriteEnable:
	begin
		if (tpw == 8'b0)
		begin
			tpw <= minTpw;
			nextState <= sWriteDisable;
		end
		else
		begin
			tpw <= tpw - 8'b1;
			nextState <= sWriteEnable;
		end
	end
	
	sWriteDisable:
	begin
		if (thd == 4'b0)
		begin
			thd <= minThd;
			nextState <= sWriteRelease;
		end
		else
		begin
			thd <= thd - 4'b1;
			nextState <= sWriteDisable;
		end
	end
	
	sWriteRelease:
	begin
	if (trest == 4'b0)
		begin
			trest <= minTrest;
			nextState <= sIdle;
		end
		else
		begin
			trest <= trest - 4'b1;
			nextState <= sWriteRelease;
		end
	end
	
	sReadSetup:
	begin
		if (tsp1 == 5'b0)
		begin
			tsp1 <= minTsp1;
			nextState <= sReadEnable;
		end
		else
		begin
			tsp1 <= tsp1 - 5'b1;
			nextState <= sReadSetup;
		end
	end
	
	sReadEnable:
	begin
		if (tpw == 8'b0)
		begin
			tpw <= minTpw;
			nextState <= sReadDisable;
		end
		else
		begin
			tpw <= tpw - 8'b1;
			nextState <= sReadEnable;
		end
	end
	
	sReadDisable:
	begin
		if (thd == 4'b0)
		begin
			thd <= minThd;
			nextState <= sReadRelease;
		end
		else
		begin
			thd <= thd - 4'b1;
			nextState <= sReadDisable;
		end
	end
	
	sReadRelease:
	begin
	if (trest == 4'b0)
		begin
			trest <= minTrest;
			nextState <= sIdle;
		end
		else
		begin
			trest <= trest - 4'b1;
			nextState <= sReadRelease;
		end
	end
	endcase
end

// state trigger
always @(posedge clk)
begin
	if (~rst_n)
		prevState <= sIdle;
	else
		prevState <= nextState;
end

endmodule

module SpiSlave(rst_n, cs, sck, mosi, miso, ctl, data, ret_data);

input rst_n, cs, sck, mosi;
input [7:0] ret_data;
output reg [7:0] ctl, data;
output reg miso;

reg [3:0] i;
reg [2:0] j;

always @(negedge rst_n or posedge cs or negedge sck)
begin
	if (~rst_n)
	begin
		miso <= 1'b0;
		i <= 4'b1111;
		j <= 3'b111;
	end
	else if (cs)
	begin
		miso <= 1'b0;
		i <= 4'b1111;
		j <= 3'b111;
	end
	else if (i > 4'b0111)
	begin	
		miso <= 1'b0;
		ctl[j] <= mosi;
		j <= j - 3'b1;
		i <= i - 4'b1;
	end
	else
	begin
		miso <= ret_data[i];
		data[i] <= mosi;
		i <= i - 4'b1;
	end
end

endmodule

