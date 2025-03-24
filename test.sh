#!/bin/bash

# Variables
UAC_FILE="uac.xml"
SERVER_IP="127.0.0.1"
SERVER_PORT="5060"
USER="1001"

# Crear el archivo uac.xml
cat > $UAC_FILE <<EOL
<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE scenario SYSTEM "sipp.dtd">

<scenario name="Basic SIP UAC">
  <send>
    <![CDATA[
    REGISTER sip:$SERVER_IP SIP/2.0
    Via: SIP/2.0/UDP [local_ip]:[local_port];branch=z9hG4bK-1
    Max-Forwards: 70
    To: <sip:$USER@$SERVER_IP>
    From: <sip:$USER@$SERVER_IP>;tag=1
    Call-ID: 1@[local_ip]
    CSeq: 1 REGISTER
    Contact: <sip:$USER@[local_ip]:[local_port]>
    Expires: 3600
    Content-Length: 0
    ]]>
  </send>

  <recv response="200" optional="true">
    <action>
      <log message="REGISTER successful!" />
    </action>
  </recv>
</scenario>
EOL

# Mostrar mensaje de creación
echo "Archivo $UAC_FILE creado con éxito."

# Ejecutar SIPp en una nueva terminal
gnome-terminal -- bash -c "sipp -sf $UAC_FILE -s $USER $SERVER_IP:$SERVER_PORT; exec bash"