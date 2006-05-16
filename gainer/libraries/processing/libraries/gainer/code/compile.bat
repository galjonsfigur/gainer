SET PROCESSING=C:\processing-0111\
SET PROCESSINGLIBS=C:\processing-0111\libraries\

javac -classpath %PROCESSING%lib\core.jar;%PROCESSINGLIBS%serial\library\RXTXcomm.jar -d %PROCESSINGLIBS%gainer\code\ Digital.java Analog.java Gainer.java
echo -Xlint:unchecked 