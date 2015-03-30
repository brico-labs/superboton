# superboton
Botón temporizador para juegos

Se trata de un botón especializado para activar juegos eléctricos, para su uso por personas con discapacidad


El temporizador debe activarse sólo cuando el usuario haya estado pulsando el botón de activación de manera ininterrumpida durante un cierto tiempo que debemos poder ajustar fácilmente (pongamos unos 5 segundos)
Cuando se produce una pulsación válida de acuerdo con lo indicado en el punto 1, se activa una salida del Arduino (ya imaginabais que iba a hacer falta, ¿verdad?) durante un tiempo que también debemos poder ajustar fácilmente. Pongamos un minuto.
Si al final del período de temporización el pulsador sigue pulsado, no se debe iniciar una nueva secuencia hasta que el usuario suelte y vuelva a pulsar el botón. De esa forma evitamos que funcione de manera continua si el botón queda atrapado accidentalmente.
Cuando se produce una pulsación válida, podemos iniciar una secuencia de luces y de música, para que el usuario sepa que ya puede soltar el botón. También podríamos hacer una vibración.