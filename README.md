# Qt VNC plugin

This is a fork of the original Qt 5 VNC platform plugin. The original plugin doesn't support rendering OpenGL content. Hence QML types from Qt Graphical Effects aren't displayed correctly. To fix that, OpenGL support was added by using OSMesa to render in software.
The original plugin only supports the RAW encoder which means that it transmits all frames uncompressed. This can be a strain on the network even for relatively small resolutions. A PNG encoder was added in order to dramatically improve bandwidth requirements.
This plugin was developed with a web client in mind that runs noVNC.
