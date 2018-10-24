// Exercise 5
// time.js  

var a = 2;
var b = 3;

function callBackFunc(x,y)
{
    		console.log("Waited 2 seconds. ");
    		console.log("The answer is: ", x*y);
};

// Now call above function after (2000 ms) 2 seconds
setTimeout(callBackFunc, 2000, a, b);
 
// Next line gets executed while waiting for the delay to complete as asynchronous
console.log("Waiting for Callback Function to complete.");