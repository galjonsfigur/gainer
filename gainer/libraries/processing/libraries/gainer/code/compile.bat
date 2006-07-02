SET PROCESSING=C:\processing-0115\
SET PROCESSINGLIBS=C:\processing-0115\libraries\

javac -target 1.3 -source 1.3 -classpath %PROCESSING%lib\core.jar;%PROCESSINGLIBS%serial\library\RXTXcomm.jar -d %PROCESSINGLIBS%gainer\code\ Digital.java Analog.java Gainer.java
echo -Xlint:unchecked 