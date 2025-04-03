# SIP Proxy Server

## Generalidades de SIP

El protocolo **SIP (Session Initiation Protocol)** es un protocolo basado en texto plano que se utiliza para la gestión de sesiones multimedia en redes IP, como llamadas de voz y videoconferencias.

### Identificación y Negociación

SIP utiliza **SDP (Session Description Protocol)** para la identificación de los flujos y la negociación de la sesión, incluyendo:

- Información de la sesión
- Número de puertos a utilizar
- Protocolos de transporte
- Códecs soportados

Para la transmisión de los flujos multimedia (audio, video, etc.), generalmente se emplean los protocolos **RTP (Real-Time Transport Protocol)** o **SRTP (Secure RTP)**. Para garantizar la seguridad en la transmisión de mensajes SIP, se utiliza **TLS (Transport Layer Security)**.

### Transporte y Puertos

SIP puede encapsularse en **TCP o UDP** y típicamente opera en los siguientes puertos:

- **5060** (sin cifrado)
- **5061** (con cifrado TLS)

### Mensajes SIP

Los mensajes SIP se dividen en dos categorías: **solicitudes (métodos)** y **respuestas**.

#### Métodos SIP (Solicitudes)

- **REGISTER**: Indica la IP actual y los URI a través de los cuales el usuario desea recibir llamadas.
- **INVITE**: Inicia una sesión multimedia entre agentes.
- **ACK**: Confirma el intercambio fiable de mensajes.
- **CANCEL**: Cancela una solicitud en curso.
- **BYE**: Finaliza una sesión en curso.
- **Otros métodos**: OPTIONS, SUBSCRIBE/NOTIFY, MESSAGE, REFER, INFO, UPDATE, PRACK.

#### Respuestas SIP

- **1xx**: Provisional
- **2xx**: Éxito
- **3xx**: Redirección
- **4xx**: Error de cliente
- **5xx**: Error de servidor
- **6xx**: Fallo global

### Establecimiento de Sesión

El establecimiento de una sesión SIP sigue el siguiente proceso:

1. El usuario llamante debe conocer la localización del usuario receptor.
2. SIP utiliza **SDP** para definir las características de la negociación, siguiendo un modelo **oferta/respuesta**.
3. El contenido SDP se incluye en el **body** del mensaje SIP.
4. Si la oferta está en el mensaje **INVITE**, la respuesta debe ser **200 OK**.
5. Si el INVITE no contiene oferta SDP, la respuesta 200 OK debe incluirla y esperar un **ACK** con la confirmación.
6. Se puede realizar una renegociación después del establecimiento de la sesión para modificar parámetros como el códec, la IP o el puerto.

SIP sigue la sintaxis definida en la **RFC 2822**.

---

## Descripción del Proyecto

Este proyecto es un **servidor proxy SIP** escrito en **C++ con características de C**, capaz de registrar usuarios e iniciar llamadas entre ellos.

### Arquitectura del Sistema

- **ServerManager**: Clase principal que gestiona el servidor.
- **SIPMessage**: Clase que almacena información importante de los mensajes SIP recibidos y facilita su procesamiento.
- **Socket**: Clase que maneja la información de los sockets **UDP** utilizados para SIP y RTP (creados con la API de sockets de Unix).
- **SIP**: Clase encargada de gestionar los mensajes SIP, generar respuestas y solicitudes y enviarlas al cliente adecuado.
- **Client** (estructura): Almacena información de los clientes registrados, incluyendo URI, dirección y estado.

---

### Optimización en Tiempo Real

Para garantizar un rendimiento óptimo y una baja latencia, se han aplicado diversas estrategias de optimización:

- Uso de UDP en lugar de TCP.

- Uso de poll() para la multiplexación.

- Sockets no bloqueantes: Se configuran los sockets para evitar bloqueos innecesarios en la recepción y envío de mensajes.

- Evitar reserva de memoria dinámica: Se minimiza la asignación de memoria en tiempo de ejecución mediante estructuras predefinidas. Uso de buffers preestablecidos.

- Optimización del procesamiento de mensajes: Se prioriza el análisis de cabeceras clave y se intentan minimizar operaciones redundantes.

- Se evitan llamadas al sistema innecesarias.

---

### Compilación

El proyecto utiliza **Makefile** para la compilación con **C++**.

---

## Mejoras y Próximos Pasos

- Implementar el **parseo y gestión de SDP** para evitar errores por incompatibilidades de sesión.
- Implementar **grupos para PTT (Push-to-Talk)**.
- Implementar la recepción y reenvío de paquetes **RTP**, permitiendo que el servidor actúe como un **B2BUA (Back-to-Back User Agent)**.
- Mejorar puntos de optimización en tiempo real.

---

## Pruebas y Compatibilidad

El servidor ha sido probado con:

- **Linphone (Ubuntu y Windows)**

- **MicroSIP (Windows)**

---
## Videos del test con linphone desde Ubuntu
- Primera parte:

![linphone_test1](https://github.com/user-attachments/assets/b169f328-cc44-4445-972f-810138f10382)


- Segunda parte:

![linphone_test2](https://github.com/user-attachments/assets/18594a65-333d-482a-a568-7e6085c6c016)

