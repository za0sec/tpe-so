# Round robin con prioridades

Cuando implementas un  **algoritmo de planificación de Round Robin con múltiples colas y prioridades** , uno de los desafíos comunes es evitar que los procesos de mayor prioridad "acaparen" toda la CPU, bloqueando los procesos de menor prioridad. Esto se llama **starvation** (inanición), y es un problema común en sistemas donde los procesos de alta prioridad reciben siempre más tiempo de CPU.

### Estrategias para evitar quedar atascado en la cola de máxima prioridad

1. **Aging (Envejecimiento)** :

* Es una técnica común para evitar el problema de starvation. Con el  **aging** , los procesos que han estado esperando mucho tiempo en las colas de menor prioridad  **ganan prioridad gradualmente** .
* Esto significa que, con el tiempo, los procesos que llevan mucho tiempo en las colas de baja prioridad eventualmente se "suben" a colas de prioridad más alta, asegurando que no se queden esperando indefinidamente.

  **Cómo implementarlo:**
* Por cada proceso en las colas de menor prioridad, incrementa su prioridad gradualmente (aumenta su prioridad después de cierto número de ciclos o ticks de reloj).
* Una vez que el proceso alcanza un umbral de prioridad, lo mueves a una cola de mayor prioridad.

  **Ejemplo de pseudocódigo:**

<pre class="!overflow-visible"><div class="dark bg-gray-950 contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative"><div class="flex items-center text-token-text-secondary bg-token-main-surface-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9">python</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-main-surface-secondary px-2 font-sans text-xs text-token-text-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-python">for each process in low_priority_queues:
       if process.wait_time > threshold:
           increase process priority
           move process to higher priority queue
   </code></div></div></pre>

1. **Asignación de tiempo de CPU a las colas de menor prioridad** :

* Puedes implementar un mecanismo que limite el tiempo que se asigna a la cola de máxima prioridad. Por ejemplo, tras procesar un cierto número de procesos o ticks de reloj de la cola de máxima prioridad, puedes decidir saltar a la siguiente cola de prioridad.

  **Cómo implementarlo:**
* Define un **quantum global** o **límite de tiempo** para cada nivel de prioridad.
* Después de procesar un número determinado de procesos de la cola de mayor prioridad, pasa a la siguiente cola.
* Esto asegura que las colas de menor prioridad también obtienen tiempo de CPU.

  **Ejemplo de pseudocódigo:**

<pre class="!overflow-visible"><div class="dark bg-gray-950 contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative"><div class="flex items-center text-token-text-secondary bg-token-main-surface-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9">python</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-main-surface-secondary px-2 font-sans text-xs text-token-text-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-python">max_priority_queue_quantum = 10  # Procesar máximo 10 ciclos en la cola de mayor prioridad
   current_cycles = 0

   while there are processes in queues:
       for each queue in priority_queues:
           while current_cycles < max_priority_queue_quantum and not queue.is_empty():
               process = queue.get_next_process()
               run_process(process)
               current_cycles += 1
           # Reiniciar los ciclos para la próxima cola
           current_cycles = 0
   </code></div></div></pre>

1. **Balanceo de tiempo entre colas** :

* Otra estrategia es asignar **proporciones de tiempo específicas** a cada cola de prioridad, de forma que se garantice que las colas de menor prioridad reciben tiempo de CPU aunque haya muchos procesos de alta prioridad.
* Por ejemplo, podrías asignar el **80% del tiempo de CPU** a las colas de alta prioridad y  **20% a las colas de baja prioridad** . Dentro de cada cola, utilizas Round Robin para distribuir el tiempo de CPU entre los procesos.

  **Cómo implementarlo:**
* Define una proporción de tiempo (cuotas) para cada cola en función de su prioridad.
* Asegúrate de que una parte del tiempo esté reservada para colas de menor prioridad, incluso si las colas de mayor prioridad aún tienen procesos en espera.

  **Ejemplo de pseudocódigo:**

<pre class="!overflow-visible"><div class="dark bg-gray-950 contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative"><div class="flex items-center text-token-text-secondary bg-token-main-surface-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9">python</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-main-surface-secondary px-2 font-sans text-xs text-token-text-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-python"># Proporciones del tiempo que asignamos a cada nivel de prioridad
   time_slice = {
       "high_priority": 0.8,
       "low_priority": 0.2
   }

   while there are processes in queues:
       run_time_for_high_priority = total_time * time_slice["high_priority"]
       run_time_for_low_priority = total_time * time_slice["low_priority"]
   
       process_high_priority_queue(run_time_for_high_priority)
       process_low_priority_queue(run_time_for_low_priority)
   </code></div></div></pre>

1. **Degradación de prioridad para procesos CPU-bound** :

* Los procesos que son **CPU-bound** (aquellos que usan intensamente la CPU y no hacen muchas operaciones de E/S) pueden ser degradados a colas de menor prioridad después de usar todo su quantum de CPU.
* Esto asegura que los procesos **I/O-bound** (que esperan a la E/S) tengan más oportunidades de ejecutar cuando estén listos.

  **Cómo implementarlo:**
* Cada vez que un proceso utiliza todo su quantum de CPU, reduces su prioridad y lo mueves a una cola de menor prioridad.

  **Ejemplo de pseudocódigo:**

<pre class="!overflow-visible"><div class="dark bg-gray-950 contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative"><div class="flex items-center text-token-text-secondary bg-token-main-surface-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9">python</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-main-surface-secondary px-2 font-sans text-xs text-token-text-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-python">if process.uses_full_quantum():
       decrease_priority(process)
       move_process_to_lower_queue(process)
   </code></div></div></pre>

### Resumen de estrategias:

1. **Aging (envejecimiento)** : Aumenta la prioridad de procesos que han estado esperando demasiado tiempo en colas de baja prioridad.
2. **Asignación de tiempo a las colas de baja prioridad** : Limita el tiempo de procesamiento en la cola de mayor prioridad y garantiza que las colas de baja prioridad también reciban tiempo de CPU.
3. **Balanceo de tiempo entre colas** : Asigna un porcentaje de tiempo fijo a cada cola según su prioridad.
4. **Degradación de procesos CPU-bound** : Reduce la prioridad de los procesos que consumen todo su quantum de CPU, dándole más oportunidades a los procesos I/O-bound.

Estas técnicas ayudan a evitar que el sistema se quede atascado procesando solo tareas de alta prioridad, asegurando que los procesos de baja prioridad también tengan tiempo de ejecución.

## Como es que se llegan a ejecutar procesos de menor prioridad?

*"o sea que siempre estoy ejecutando procesos de la cola con mayor prioridad, y lo que hace que se ejecuten procesos de menor prioridad es que yo SUBO la prioridad de los que tenian menor prioridad, o BAJO la prioridad de los que tenian mayor. Quiero decir, en ningun momento elijo ejecutar procesos de una prioridad N si es que tengo procesos en la cola de prioridad N - 1 (donde un N mas alto significa prioridad menor)"*

###### ChatGPT said:

Correcto, lo que estás describiendo es una **planificación estricta basada en prioridades** con **envejecimiento (aging)** o **degradación de prioridad** para evitar la inanición ( **starvation** ). En este tipo de planificación, **siempre** ejecutas procesos de la cola con la prioridad más alta, y solo consideras procesos de colas de menor prioridad cuando las colas de mayor prioridad están vacías o los procesos en esas colas han sido degradados.

### Esquema de Funcionamiento:

1. **Procesos en la cola de máxima prioridad** :

* Siempre que haya procesos en la cola de mayor prioridad, se ejecutan primero. No consideras procesos de colas de menor prioridad hasta que la cola de mayor prioridad esté vacía.

1. **Degradación de prioridad para procesos CPU-bound** :

* Si un proceso en la cola de mayor prioridad consume todo su **quantum** de CPU, su prioridad puede ser **reducida** (degradada), y el proceso se mueve a una cola de menor prioridad.
* Esto asegura que un proceso que requiere mucho tiempo de CPU no se quede indefinidamente en la cola de mayor prioridad, permitiendo que otros procesos tengan oportunidad de ejecutarse.

1. **Envejecimiento (aging)** :

* Los procesos en colas de menor prioridad **aumentan su prioridad gradualmente** cuanto más tiempo esperan, para evitar que se queden bloqueados indefinidamente.
* Después de esperar un tiempo suficiente, un proceso de una cola de baja prioridad puede moverse a una cola de mayor prioridad, eventualmente ganando suficiente prioridad para ejecutarse.

### Flujo de Decisión:

* **Siempre se ejecuta el proceso de mayor prioridad disponible** :
* Si tienes procesos en la cola de prioridad 1 (la más alta), siempre ejecutarás esos procesos.
* Solo si la cola de prioridad 1 está vacía, mirarás la cola de prioridad 2, y así sucesivamente.
* **Envejecimiento y degradación de prioridad** :
* Los procesos en colas de menor prioridad **aumentan su prioridad con el tiempo** (envejecen).
* Los procesos que consumen mucho CPU y utilizan todo su quantum **pierden prioridad** y se mueven a una cola de menor prioridad.

### Ejemplo:

Imagina que tienes tres colas de prioridad: 1 (alta), 2 (media), y 3 (baja).

1. **Proceso en prioridad 1** :

* Ejecutas siempre un proceso de la cola de prioridad 1 si hay alguno disponible.
* Si el proceso usa todo su quantum de CPU, puede ser degradado a la cola de prioridad 2.

1. **Proceso en prioridad 2** :

* Si no hay procesos en la cola de prioridad 1, ejecutas procesos de la cola de prioridad 2.
* Si el proceso usa todo su quantum, podría ser degradado a la cola de prioridad 3.

1. **Proceso en prioridad 3** :

* Si las colas de prioridad 1 y 2 están vacías, entonces ejecutas procesos de la cola de prioridad 3.
* Un proceso en la cola de prioridad 3 podría "envejecer" y ganar prioridad, moviéndose eventualmente a la cola de prioridad 2 o incluso a la cola de prioridad 1.

### Importancia del Quantum en el Mecanismo:

* El **quantum** es clave en este sistema. Los procesos **CPU-bound** (que consumen todo el quantum) tienden a ser degradados a colas de menor prioridad, mientras que los procesos **I/O-bound** (que liberan la CPU antes de consumir todo el quantum) tienden a permanecer en colas de mayor prioridad, ya que requieren menos CPU y suelen estar bloqueados esperando E/S.

### Consideraciones:

1. **Inanición (starvation)** :

* Si no se implementa envejecimiento o degradación adecuada, los procesos de baja prioridad pueden **nunca ejecutarse** si hay un flujo constante de procesos de alta prioridad.

1. **Balance entre CPU-bound e I/O-bound** :

* Los procesos CPU-bound tienden a bajar de prioridad más rápido, ya que usan mucho tiempo de CPU.
* Los procesos I/O-bound generalmente se completan más rápido y pueden permanecer en colas de mayor prioridad si no consumen todo su quantum.

### Resumen:

* **Nunca** eliges ejecutar procesos de menor prioridad si hay procesos en la cola de mayor prioridad.
* Lo que permite que los procesos de menor prioridad eventualmente se ejecuten es:
  1. **Envejecimiento (aging)** de los procesos de menor prioridad, que incrementa su prioridad con el tiempo.
  2. **Degradación** de los procesos de mayor prioridad que consumen mucho tiempo de CPU, moviéndolos a colas de menor prioridad.

# Cola de bloqueados

Sí, almacenar los procesos bloqueados en una segunda cola es una buena idea y es una práctica común en los sistemas operativos. Al tener una **segunda cola** específica para procesos bloqueados, puedes gestionar los procesos que están esperando por algún evento de manera más eficiente. Esto te permite separar los procesos que están listos para ejecutarse de aquellos que están esperando por I/O u otro evento.

### Beneficios de usar una cola para procesos bloqueados:

1. **Separación de responsabilidades** :

* Mantienes la **cola de procesos listos** (`process_queue`) solo con los procesos que pueden ejecutarse inmediatamente.
* Los procesos en estado **`BLOCKED`** no necesitan ser evaluados constantemente por el scheduler, ya que no pueden ser ejecutados hasta que el evento que los bloquea se resuelva.

1. **Mejora de la eficiencia** :

* Evitas que el scheduler tenga que verificar constantemente el estado de los procesos bloqueados en cada iteración, lo cual sería ineficiente.
* Solo cuando un proceso bloqueado se desbloquea, lo mueves a la cola de listos (`process_queue`), y el scheduler lo podrá ejecutar.

1. **Mejor manejo del estado** :

* Puedes manejar el desbloqueo de procesos de manera más sencilla: cuando un evento de I/O u otro recurso se desbloquea, puedes simplemente mover el proceso correspondiente de la cola de bloqueados a la cola de listos.

### Implementación sugerida:

#### 1. Crear una cola separada para procesos bloqueados:

Puedes crear una segunda cola, similar a `process_queue`, para almacenar los procesos bloqueados.

<pre class="!overflow-visible"><div class="dark bg-gray-950 contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative"><div class="flex items-center text-token-text-secondary bg-token-main-surface-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9">c</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-main-surface-secondary px-2 font-sans text-xs text-token-text-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-c">q_adt blocked_queue = NULL;
</code></div></div></pre>

#### 2. Modificar el scheduler para mover procesos a la cola de bloqueados:

Cuando un proceso cambia su estado a  **`BLOCKED`** , en lugar de reinsertarlo en la cola de listos, lo mueves a la cola de bloqueados.

<pre class="!overflow-visible"><div class="dark bg-gray-950 contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative"><div class="flex items-center text-token-text-secondary bg-token-main-surface-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9">c</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-main-surface-secondary px-2 font-sans text-xs text-token-text-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-c">if (current_process.state == BLOCKED) {
    // Mover el proceso a la cola de bloqueados en lugar de la de listos
    add(blocked_queue, current_process);
}
</code></div></div></pre>

#### 3. Manejar el desbloqueo de procesos:

Cuando un proceso bloqueado es desbloqueado (por ejemplo, porque terminó un evento de I/O), lo mueves de la cola de bloqueados a la cola de listos.

Puedes tener una función que recorra la cola de procesos bloqueados y verifique si algún evento ha desbloqueado los procesos.

<pre class="!overflow-visible"><div class="dark bg-gray-950 contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative"><div class="flex items-center text-token-text-secondary bg-token-main-surface-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9">c</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-main-surface-secondary px-2 font-sans text-xs text-token-text-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-c">void check_blocked_processes() {
    q_adt temp_queue = new_q();  // Cola temporal para almacenar los procesos que no se desbloquearon
    while (has_next(blocked_queue)) {
        pcb_t blocked_process = dequeue(blocked_queue);
        if (event_unblocked(blocked_process)) {
            blocked_process.state = READY;
            add(process_queue, blocked_process);  // Mover a la cola de listos
        } else {
            add(temp_queue, blocked_process);  // Mantener en la cola de bloqueados
        }
    }
    // Reemplazar la cola de bloqueados con la cola temporal
    free_q(blocked_queue);
    blocked_queue = temp_queue;
}
</code></div></div></pre>

#### 4. Modificar `schedule()` para manejar bloqueos:

En el `schedule()`, antes de elegir el siguiente proceso, puedes llamar a `check_blocked_processes()` para mover procesos desbloqueados a la cola de listos.

<pre class="!overflow-visible"><div class="dark bg-gray-950 contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative"><div class="flex items-center text-token-text-secondary bg-token-main-surface-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9">c</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-main-surface-secondary px-2 font-sans text-xs text-token-text-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-c">uint64_t schedule(uint64_t running_process_rsp) {
    if (process_queue == NULL) {
        init_schedule();
    }

    // Guardar el RSP del proceso actual
    current_process.rsp = running_process_rsp;

    // Manejar el estado del proceso actual
    if (current_process.state == RUNNING) {
        current_process.used_quantum++;
        if (current_process.used_quantum < current_process.assigned_quantum) {
            return current_process.rsp;
        } else {
            current_process.state = READY;
            current_process.used_quantum = 0;
            current_process.assigned_quantum--;
            add(process_queue, current_process);
        }
    } else if (current_process.state == BLOCKED) {
        add(blocked_queue, current_process);
    } else if (current_process.state == TERMINATED) {
        cleanup_process(&current_process);
    }

    // Verificar si hay procesos bloqueados que puedan desbloquearse
    check_blocked_processes();

    // Obtener el siguiente proceso listo para ejecutar
    if (!has_next(process_queue)) {
        create_halt_process();
    } else {
        current_process = dequeue(process_queue);
        current_process.state = RUNNING;
    }

    return current_process.rsp;
}
</code></div></div></pre>

#### 5. Desbloqueo basado en eventos:

Tu implementación necesitará algún mecanismo para desbloquear procesos. Por ejemplo, si estás esperando por I/O, una interrupción podría llamar a `check_blocked_processes()` para revisar si algún proceso puede desbloquearse.

Este desbloqueo puede ocurrir como respuesta a una interrupción de hardware, un temporizador o cualquier otro evento que el proceso esté esperando.

#### 6. Resumen del flujo:

1. Los procesos bloqueados se almacenan en una cola separada.
2. Cada vez que el `schedule()` se ejecuta, verifica si hay procesos que puedan ser desbloqueados.
3. Si un proceso se desbloquea, se mueve de la cola de bloqueados a la cola de listos.
4. El scheduler sigue funcionando con los procesos listos, y solo revisa la cola de bloqueados cuando un evento puede desbloquear algún proceso.

### Conclusión:

Almacenar los procesos bloqueados en una segunda cola (como `blocked_queue`) te permitirá gestionar mejor los procesos que esperan por eventos, evitando que el scheduler tenga que revisar continuamente procesos que no pueden ser ejecutados. Esta mejora hará que tu sistema de planificación sea más eficiente y modular, facilitando el manejo de procesos en diferentes estados (`READY`, `RUNNING`, `BLOCKED`, `TERMINATED`).
