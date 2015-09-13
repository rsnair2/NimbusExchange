
# Build the Exchange!
Sponsored by TransmarketGroup LLC

### Overview
In this exercise, you will be implementing an order matching engine -- the heart of any modern electronic exchange. The Matching Engine is an algorithm implemented in software that matches customers' buying and selling instructions. 

To buy or to sell an exchange listed security customers send their instructions to the exchange in the form of orders. Order has a unique id, security identifier - a symbol, a price at witch customer is ready to transact and the desired size. When a buyer and seller are matched - the exchange generates a FILL - an instruction sent to both buyer and seller informing them of the match. 

If the arriving order cannot be matched immediately, it is put on the book. The orders are organized in terms of price/time priority. Orders with better prices are ahead of orders with worse prices and orders that arrived first for the same price are ahead of orders that arrived later. 

If incoming order price is worse than the order resting on the book - then the FILL is generated at better resting price rather than at incoming price. 

Customers can also amend their orders by cancelling or replacing them. When it is just the order size that changes - the order gets to keep its position in the queue, if the order price changes as well, then the order is added at the end of the queue for the new price. 

Customers can also cancel their orders all together by sending CANCEL instruction with the order id.

In the matching engine you are building today, you will be supporting the following messages:

-   ORDER -> The limit order is the standard order type to be supported. It represents a Bid or Offer (Buy or Sell).
-   CANCEL -> The cancel is represented as a full cancel to a limit order in the market. A cancel order removes a single order from the Market.
-   REPLACE -> The replace is a change of a quantity and(or) price of an order for the same side in a market. A replace order can do 1 of 2 things:
    -   If the price is the same and the quantity goes down, this can be thought of as a partial cancel (cancel down). Queue position is maintained in this case.
    -   If the price is different *or* the quantity increases, this should be handled as a Cancel of the previous order and a new ORDER order should be made. Queue position isn't maintained in this case.

### Things to Consider

Please take note of the following cases in your design

-	For a given price level, orders are to be filled are in a time-based priority. The first orders encountered should be filled first for a given price-level.
-	When having an order that crosses the book and thus results in a fill. The price of the fill is based upon the *best* order on the other side of the book.
-	Fills are centered around an order matching with another order (result fills contain both OrderIDs after all). If a crossing order can fill multiple orders at the same time, the fill *should not* be aggregated into a single fill.
-	In the case where a crossing order can fill an entire price level on the other side of the book, you must continue to iterate among other price levels that the crossing order crosses, until the crossing order doesn't cross the book.

### File Formats
We have provided two pairs of files which represent orders to be sent to the exchange and the fills (trades) that are expected from an order matching engine(one pair acts as a simple example case, and another pair that acts as a longer, more complex case -- which your implementation will be tested against).  
#### Input Format
The input file format is outlined below. *Your final executable should allow for a filename to be passed into it*. The file format is **csv**.

The file will contain records of Order Types -- to be executed sequentially. Each record has the following format

Order_Type,OrderID,Symbol,Side,Quantity,Price,Old Quantity\*,Old Price\*

*\*Used only for REPLACE*
where

- Order_Type -> [ORDER, CANCEL, REPLACE]
- OrderID -> Unique Integer for Order, allows Replaces and Cancels to lookup which Order they want to Cancel or Replace.
- Symbol -> Unique String to Represent Product -- has its own order book.
- Side -> (B|S) Represents an order to BUY or SELL
- Quantity -> Integer representing number of product to BUY or SELL.  If REPLACE, this represents the new Quantity.
- Price -> Double representing price at which an order will BUY or SELL something at. If REPLACE, this represents the new Price.

Some examples are below:

ORDER,7,GS,S,8,73.0  
ORDER,8,GS,B,3,81.0  
ORDER,9,GS,S,4,66.5  
CANCEL,1,AAPL,B,5,65.5  
ORDER,10,GS,S,7,81.5  
ORDER,11,AAPL,B,9,74.5  
ORDER,12,F,S,10,89.5  
ORDER,13,F,B,8,76.5  
ORDER,14,GS,B,14,59.5  
ORDER,15,AAPL,S,11,77.0  
ORDER,16,F,B,9,60.5  
ORDER,17,AAPL,B,3,65.5  
ORDER,18,F,S,22,76.0  
ORDER,19,GS,B,14,77.0  
ORDER,20,F,B,18,71.0  
REPLACE,17,AAPL,B,15,76.5,3,65.5  
ORDER,21,AAPL,B,11,49.5  
ORDER,22,GS,S,8,66.0  
ORDER,23,AAPL,B,18,74.5  
ORDER,24,GS,B,13,66.5  
ORDER,25,GS,S,10,84.0  
ORDER,26,F,B,15,76.0  
ORDER,27,GS,B,15,75.5  
ORDER,28,GS,B,6,51.0  

#### Output Format
Output should be written to a file named fills.txt. Output should only have FILL records, which represent trades that the matching engine produces. FILL Records have the following format, which are space delimited.

FILL Symbol BuySideOrderID SellSideOrderID Quantity Price

where

-   Symbol -> Symbol that was traded
-   BuySideOrderID -> The buy order that was involved with one side of the trade.
-   SellSideOrderID -> The sell order that was involved with the other side of the trade.
-   Quantity -> The amount that got filled/traded.
-   Price -> The price of which the fill occurs.

Some examples are below:

FILL GS 34 32 5 70.0  
FILL AAPL 17 15 7 76.5  
FILL F 26 53 1 76.0  
FILL F 16 53 2 73.5  
FILL F 3 53 1 71.0  
FILL GS 40 32 4 70.0  
FILL GS 40 10 7 81.5  
FILL GS 40 25 3 84.0  
FILL F 57 41 2 79.5  
FILL F 57 61 1 80.5  
FILL F 57 64 5 80.5  

### Program Usage
The program should take in an order input file, and produce a fills.txt file:  

Usage:  
./MatchingEngine <infile>   

### Building
A CMakeLists.txt and generate script have been provided.  

To start, simply run the generate script:  
./generate_release_build.sh  (for release)  
./generate_debug_build.sh  (for debug)  

Then the project can be built from the /build directory:  
cd build  
make  

This should produce an executable name 'MatchingEngine' that can be run.

### Grading

Implementations will be graded on two metrics: first correctness and then speed.

-  Correctness: The matching engine's ability to produce FILLs with the same attributes, in the same order. We will judge correctness before speed.
-  Speed: Simply the fastest implementation using the linux time command will win

