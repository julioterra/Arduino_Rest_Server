RestServer Library
==================
   
This simple library was designed to enable your Arduino to respond to RESTful resource requests via Ethernet. Unlike most existing RESTful libraries for the Arduino, RestServer enables you to describe your own resources and to define how the Arduino should respond to each resource request rather than automatically map resources to pin numbers on the Arduino.  
  
The intent of this design approach is to embed knowledge about the resources available on a given Arduino locally on the device itself. This local knowledge enables the Arduino to inform client devices about the resources are available, along with relevant information about the resources such as resource range and types of requests supported.  
  
For more information about how this library works [check out the wiki](https://github.com/julioterra/Arduino_Rest_Server/wiki)
   
@author Julio Terra
@modified 12/25/12 by Julio
@version 1.0.2

  
License  
=======  
  
The MIT License (MIT)  
Copyright © 2012 LAB at Rockwell Group, http://www.rockwellgroup.com/lab  
  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  