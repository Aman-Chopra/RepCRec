make
./build/RepCRec tests/inputs/input5

Notes:
Please add dump() after every test case to print the value of all the variables at all sites. All other events are printed 
whenever an event happens.
Normally, we wait till the end(T1) to abort T1 if it accessed a site which went down, so reads are successful and and writes will not be persistent at the commit time.