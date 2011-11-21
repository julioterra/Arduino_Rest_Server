# RestServer Library
  
This simple library was designed to enable your Arduino to respond to RESTful resource requests via Ethernet or Serial connections. Unlike most existing RESTful libraries for the Arduino, RestServer enables you to describe your own resources and to define how the Arduino should respond to each resource request rather than automatically map resources to pin numbers on the Arduino.  
  
The intent of this design approach is to embed knowledge about the resources available on a given Arduino locally on the device itself. This local knowledge enables the Arduino to inform client devices about the resources are available, along with relevant information about the resources such as resource range and types of requests supported.

For more information about how this library works [check out the wiki](https://github.com/julioterra/Arduino_Rest_Server/wiki)