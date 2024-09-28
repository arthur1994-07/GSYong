local myClass = MyClass();                           
local rVal = myClass.process(1,"MyClass1");        
print("Returned: "+(rVal ? "true" : "false")); 
rVal = myClass.process(2,"MyClass2");              
print("Returned: "+(rVal ? "true" : "false")); 
print("classVal: "+myClass.classVal);              
