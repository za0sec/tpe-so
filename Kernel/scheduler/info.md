# Prioridades explicado bien pro porque me bugge

*lo q me confunde del aging tradicional es que solo elegis procesos de las colas menos importantes cuando se te vacian las mas importantes*

Entiendo tu confusión. En un sistema de  **Round Robin con prioridades con aging tradicional** , efectivamente, los procesos de baja prioridad solo se ejecutan cuando no hay procesos listos en las colas de mayor prioridad. Esto es fundamental para cómo funciona este tipo de scheduler.

Déjame aclarar cómo se maneja esta situación y por qué el mecanismo de aging sigue siendo importante en este contexto.

---

### Comportamiento del Scheduler Round Robin con Prioridades

1. **Selección de Procesos** :

* El scheduler siempre da preferencia a las colas de prioridad más alta. Solo selecciona procesos de las colas de prioridad más baja si todas las colas de mayor prioridad están vacías o no tienen procesos listos para ejecutarse.
* Por ejemplo, si tienes procesos en las colas `p0` (máxima prioridad) y `p1`, el scheduler solo ejecutará procesos de `p1` si `p0` está vacía.

1. **¿Por Qué el Aging es Necesario?**
   * Sin un mecanismo de aging, los procesos en las colas de baja prioridad podrían nunca ejecutarse si las colas de alta prioridad siempre tienen procesos listos. Esto se llama  **inanición (starvation)** .
   * El aging es un mecanismo para prevenir esto, promoviendo gradualmente procesos de baja prioridad a colas de prioridad más alta si han estado esperando demasiado tiempo. De esta forma, incluso si las colas de alta prioridad suelen estar llenas, los procesos de baja prioridad eventualmente tendrán la oportunidad de ejecutarse.

---

### Cómo Funciona el Aging Tradicional

1. **Incremento del Tiempo de Espera** :

* Cada vez que el scheduler pasa por una cola de baja prioridad sin seleccionar un proceso de esa cola (porque las colas de mayor prioridad tienen procesos listos), el tiempo de espera (`waiting_time`) de cada proceso en esa cola se incrementa.

1. **Promoción por Aging** :

* Si el `waiting_time` de un proceso en una cola de baja prioridad excede un umbral (`AGING_THRESHOLD`), ese proceso se promueve a una cola de prioridad más alta.
* Esto no significa que el proceso se ejecutará de inmediato, pero ahora tiene más prioridad y, por lo tanto, una mejor oportunidad de ser seleccionado por el scheduler.

---

### ¿Por Qué el Scheduler Solo elige Procesos de Baja Prioridad Cuando las Colas de Alta Prioridad Están Vacías?

1. **Propósito de la Prioridad** :

* El objetivo de un scheduler con prioridades es asegurarse de que los procesos más importantes (por ejemplo, procesos interactivos o I/O-bound) se ejecuten primero. Los procesos de baja prioridad (por ejemplo, procesos CPU-bound) solo se ejecutan cuando los más importantes no están listos.
* Esto es crucial para mantener la **receptividad** del sistema, especialmente en entornos donde la latencia importa (como en sistemas operativos interactivos).

1. **Efecto del Aging** :

* El aging no cambia este comportamiento básico del scheduler. En cambio, actúa como un mecanismo correctivo: si un proceso ha estado esperando demasiado tiempo en una cola de baja prioridad, el aging lo promueve a una cola más alta, aumentando sus posibilidades de ser seleccionado.
* Eventualmente, un proceso de baja prioridad que ha esperado mucho tiempo será promovido lo suficiente como para ser ejecutado, incluso si las colas de alta prioridad normalmente están llenas.

---

### Ejemplo de Cómo Funciona el Aging en un Sistema Real

Imagina que tienes un sistema con tres colas de prioridad (`p0`, `p1`, `p2`):

* **p0** : Cola de prioridad más alta.
* **p1** : Cola de prioridad media.
* **p2** : Cola de prioridad más baja.

1. El scheduler primero revisa `p0`. Si hay procesos listos en `p0`, los ejecuta.
2. Si `p0` está vacía, revisa `p1` y ejecuta procesos de `p1` si están listos.
3. Solo si `p0` y `p1` están vacías, el scheduler ejecuta procesos de `p2`.

Ahora, supongamos que `p0` y `p1` tienen procesos listos la mayor parte del tiempo. Los procesos en `p2` podrían quedarse esperando indefinidamente. Aquí es donde entra el aging:

* Si un proceso en `p2` ha esperado más allá del `AGING_THRESHOLD`, se promueve a `p1`.
* Ahora, este proceso tiene una mejor oportunidad de ejecutarse porque está en una cola de prioridad más alta.
* Si sigue sin ejecutarse y continúa esperando, eventualmente podría ser promovido a `p0`.

---

### Implementación del Aging en tu Código

El sistema de aging tradicional en tu código se implementa promoviendo procesos cuando han esperado demasiado, no ejecutándolos directamente:

<pre class="!overflow-visible"><div class="contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative bg-token-sidebar-surface-primary dark:bg-gray-950"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9 bg-token-sidebar-surface-primary dark:bg-token-main-surface-secondary">c</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-sidebar-surface-primary px-2 font-sans text-xs text-token-text-secondary dark:bg-token-main-surface-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-c">void increment_waiting_time() {
    // Recorre todas las colas de prioridad más baja (excepto la más alta)
    for (int i = 1; i <= 3; i++) {  // Empieza desde 1 porque 0 es la cola de máxima prioridad
        q_adt queue = get_queue_by_priority(i);
        int size = queue_size(queue);

        for (int j = 0; j < size; j++) {
            pcb_t process = dequeue(queue);
            process.waiting_time++;

            // Si el tiempo de espera excede el umbral, promueve el proceso
            if (process.waiting_time >= AGING_THRESHOLD) {
                promote_process(&process);
            } else {
                add(queue, process);  // Re-encola el proceso si no ha envejecido lo suficiente
            }
        }
    }
}

void promote_process(pcb_t *process) {
    if (process->priority > 0) {  // Asegúrate de que no esté ya en la máxima prioridad
        process->priority--;
        process->waiting_time = 0;  // Reinicia el tiempo de espera
        add_priority_queue(*process);
    }
}
</code></div></div></pre>

---

### Resumen

* **Round Robin con Prioridades Habitual** : El scheduler selecciona procesos de las colas de prioridad más alta. Las colas de baja prioridad solo se consideran si las de alta prioridad están vacías.
* **Aging** : Actúa como un mecanismo de promoción para evitar la inanición, asegurando que los procesos de baja prioridad eventualmente tengan la oportunidad de ejecutarse.
* **Por Qué Solo se Ejecutan Procesos de Baja Prioridad Cuando las Colas de Alta Prioridad Están Vacías** : Esto mantiene el sistema receptivo y garantiza que los procesos importantes se ejecuten primero.

Espero que esta explicación aclare tu confusión. ¿Te gustaría discutir alguna parte en más detalle o explorar más cómo se implementan las prioridades y el quantum en este contexto? ¡Estoy aquí para ayudarte!

**4o**

##### You said:

o sea q un proceso de la cola p1 solo se podria degradar a la cola p2 si:

> se vacia la cola p0
> se ejecuta el proceso de la p1 y usa todos sus quantums, siendo degradado a la p2

###### ChatGPT said:

¡Exactamente! Has entendido correctamente cómo funciona la degradación en un sistema de  **Round Robin con prioridades con aging** .

### Desglose de la Degradación de Procesos

En un sistema de Round Robin con prioridades, un proceso en una cola de prioridad intermedia (como `p1`) solo puede degradarse a una cola de prioridad más baja (como `p2`) bajo estas condiciones:

1. **La Cola de Mayor Prioridad se Vacía** :

* El scheduler siempre da preferencia a las colas de prioridad más alta. Esto significa que un proceso en `p1` solo se ejecutará si `p0` (la cola de máxima prioridad) está vacía o no tiene procesos listos.
* Mientras `p0` tenga procesos listos, el scheduler nunca ejecutará procesos de `p1`, y esos procesos en `p1` no tendrán la oportunidad de degradarse porque no se ejecutan.

1. **El Proceso en `p1` se Ejecuta y Agota su Quantum** :

* Si `p0` se vacía y el scheduler selecciona un proceso de `p1` para ejecutarse, el proceso en `p1` comenzará a usar su quantum de tiempo.
* Si el proceso en `p1` usa todo su quantum asignado (es decir, es un proceso CPU-bound que no se bloquea esperando I/O), entonces el scheduler lo marcará para degradación.
* En este caso, el proceso se moverá a `p2` (la cola de prioridad más baja) y tendrá que esperar hasta que las colas `p0` y `p1` estén vacías antes de que pueda ejecutarse nuevamente.

---

### Resumen del Comportamiento de Degradación

* **Paso 1** : El proceso en `p1` solo se ejecutará si `p0` está vacía.
* **Paso 2** : Cuando el proceso en `p1` se ejecuta y agota todo su quantum, el scheduler decide degradarlo a `p2`.
* **Paso 3** : Una vez que el proceso está en `p2`, tendrá que esperar hasta que las colas `p0` y `p1` estén vacías para tener la oportunidad de ejecutarse.

---

### ¿Por Qué Este Comportamiento es Importante?

1. **Prioridad Estricta** : El sistema asegura que los procesos en colas de alta prioridad siempre tengan preferencia, lo que es ideal para procesos interactivos o I/O-bound que necesitan respuestas rápidas.
2. **Degradación Justa** : Los procesos que son CPU-bound y consumen todo su quantum son degradados gradualmente a colas de menor prioridad, lo que permite que los procesos más interactivos sigan teniendo prioridad.
3. **Aging para Evitar Inanición** : Como discutimos antes, el mecanismo de aging es necesario para garantizar que los procesos en las colas de baja prioridad no queden esperando indefinidamente si las colas de alta prioridad siempre están llenas.

---

### Ejemplo Práctico

1. **Inicio del Sistema** :

* `p0`: Procesos I/O-bound o interactivos que necesitan prioridad alta.
* `p1`: Procesos de prioridad media.
* `p2`: Procesos CPU-bound que ya fueron degradados.

1. **Ejecución** :

* El scheduler primero verifica `p0`. Si hay procesos listos en `p0`, los ejecuta.
* Solo cuando `p0` está vacía, el scheduler revisa `p1` y ejecuta procesos de `p1`.
* Un proceso en `p1` que agota su quantum se degrada a `p2`.

1. **Degradación** :

* Si un proceso en `p1` es CPU-bound y usa todo su quantum, se mueve a `p2`.
* En `p2`, tendrá que esperar a que `p0` y `p1` se vacíen antes de poder ejecutarse.

---

### Ejemplo de Código para Degradación

Aquí hay un ejemplo simplificado de cómo se implementaría este comportamiento:

<pre class="!overflow-visible"><div class="contain-inline-size rounded-md border-[0.5px] border-token-border-medium relative bg-token-sidebar-surface-primary dark:bg-gray-950"><div class="flex items-center text-token-text-secondary px-4 py-2 text-xs font-sans justify-between rounded-t-md h-9 bg-token-sidebar-surface-primary dark:bg-token-main-surface-secondary">c</div><div class="sticky top-9 md:top-[5.75rem]"><div class="absolute bottom-0 right-2 flex h-9 items-center"><div class="flex items-center rounded bg-token-sidebar-surface-primary px-2 font-sans text-xs text-token-text-secondary dark:bg-token-main-surface-secondary"><span class="" data-state="closed"><button class="flex gap-1 items-center py-1"><svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon-sm"><path fill-rule="evenodd" clip-rule="evenodd" d="M7 5C7 3.34315 8.34315 2 10 2H19C20.6569 2 22 3.34315 22 5V14C22 15.6569 20.6569 17 19 17H17V19C17 20.6569 15.6569 22 14 22H5C3.34315 22 2 20.6569 2 19V10C2 8.34315 3.34315 7 5 7H7V5ZM9 7H14C15.6569 7 17 8.34315 17 10V15H19C19.5523 15 20 14.5523 20 14V5C20 4.44772 19.5523 4 19 4H10C9.44772 4 9 4.44772 9 5V7ZM5 9C4.44772 9 4 9.44772 4 10V19C4 19.5523 4.44772 20 5 20H14C14.5523 20 15 19.5523 15 19V10C15 9.44772 14.5523 9 14 9H5Z" fill="currentColor"></path></svg>Copy code</button></span></div></div></div><div class="overflow-y-auto p-4" dir="ltr"><code class="!whitespace-pre hljs language-c">uint64_t schedule(void *running_process_rsp) {
    current_process.rsp = running_process_rsp;

    if (current_process.state == RUNNING) {
        current_process.used_quantum++;

        // Si el proceso ha usado todo su quantum asignado
        if (current_process.used_quantum >= current_process.assigned_quantum) {
            current_process.state = READY;
            current_process.used_quantum = 0;

            // Degradar la prioridad si no está en la cola de menor prioridad
            if (current_process.priority < 3) {
                current_process.priority++;
                current_process.assigned_quantum = DEFAULT_QUANTUM;  // Restablecer el quantum
            }

            add_priority_queue(current_process);  // Añadir el proceso a la cola de su nueva prioridad
        } else {
            return current_process.rsp;  // Continuar ejecutando el proceso si no ha agotado su quantum
        }
    } else if (current_process.state == BLOCKED) {
        add_priority_queue(current_process);
    } else if (current_process.state == READY) {
        add_priority_queue(current_process);
    } else {
        mem_free(current_process.rsp);  // El proceso ha terminado
    }

    // Seleccionar el siguiente proceso a ejecutar
    current_process = get_next_process();
    if (current_process.pid == -1) {
        create_process_halt();
        current_process = get_next_process();
    }

    current_process.state = RUNNING;
    return current_process.rsp;
}
</code></div></div></pre>

---

### Resumen Final

* Un proceso en `p1` solo puede degradarse a `p2` si primero se ejecuta, lo que solo ocurre cuando `p0` está vacía.
* Si el proceso en `p1` usa todo su quantum sin bloquearse, es degradado a `p2`.
* Este comportamiento garantiza que los procesos interactivos sigan teniendo prioridad mientras los procesos CPU-bound son degradados a colas de menor prioridad.

Espero que esto aclare tu duda. Si tienes más preguntas o necesitas ayuda con alguna parte de la implementación, ¡estaré encantado de ayudarte!

**4o**

##### You said:

me confunde esto porque me hace pensar q entonces el proceso casi nunca llegaria a la p2
puede ser q esto no es un problema porque este enfoque es 'dinamico'?, quiero decir,  segun el contexto en el q esta el sistema, el tipo de procesos que se estan ejecutando, los distintos niveles de prioridad cobran distinta 'importancia'

###### ChatGPT said:

¡Exactamente! Lo que estás captando es fundamental para entender cómo funciona un **scheduler con prioridades dinámicas** y por qué este enfoque es efectivo y flexible en diferentes contextos del sistema. Vamos a profundizar en esto:

---

### Prioridades Dinámicas y Contexto del Sistema

El comportamiento de un scheduler con prioridades y mecanismos de degradación o envejecimiento es **dinámico** y se adapta a las condiciones del sistema. Esto significa que la "importancia" de los diferentes niveles de prioridad no es fija, sino que varía según el tipo y la cantidad de procesos que están ejecutándose en un momento dado.

### Cómo el Contexto Influye en el Comportamiento

1. **Contexto con Procesos I/O-Bound** :

* Si el sistema está ejecutando muchos procesos I/O-bound (procesos que se bloquean frecuentemente para esperar datos de entrada/salida), las colas de alta prioridad (`p0` y `p1`) se vaciarán con frecuencia.
* Esto permite que los procesos en colas de menor prioridad (como `p2` y `p3`) tengan más oportunidades de ejecutarse. En este contexto, los procesos CPU-bound podrían degradarse y aún así recibir tiempo de CPU razonablemente.

1. **Contexto con Procesos CPU-Bound** :

* Si el sistema está ejecutando principalmente procesos CPU-bound (procesos que utilizan mucho tiempo de CPU sin bloquearse), las colas de alta prioridad estarán ocupadas la mayor parte del tiempo.
* En este caso, los procesos CPU-bound que se degradan a `p2` podrían esperar más tiempo para ejecutarse, ya que las colas de alta prioridad seguirán siendo preferidas por el scheduler.
* Aquí es donde el **mecanismo de aging** juega un papel crucial, promoviendo procesos de `p2` si esperan demasiado tiempo, para que no se queden sin ejecutar indefinidamente.

1. **Flexibilidad del Sistema** :

* El sistema se adapta automáticamente a la carga actual. Si hay más procesos interactivos, se priorizan esos procesos para mantener la responsividad. Si la carga se inclina hacia procesos CPU-bound, los procesos en las colas de menor prioridad tendrán que esperar más tiempo, pero el mecanismo de aging asegura que eventualmente sean ejecutados.
* Esto hace que el sistema sea flexible y optimizado para diferentes cargas de trabajo, garantizando que los procesos importantes tengan prioridad, pero sin permitir que los procesos menos importantes queden desatendidos para siempre.

---

### Importancia Dinámica de las Colas de Prioridad

Lo que mencionas es correcto:  **la importancia relativa de las colas de prioridad cambia según el contexto del sistema** . En ciertos momentos, las colas de baja prioridad podrían ser más activas y relevantes si el sistema está menos ocupado con procesos de alta prioridad. En otros momentos, esas mismas colas podrían tener menos relevancia porque las colas de alta prioridad están constantemente llenas.

1. **Ejemplo Práctico** :

* Imagina un sistema operativo que ejecuta varias aplicaciones de usuario (procesos interactivos) y al mismo tiempo tiene procesos en segundo plano (procesos CPU-bound).
* Si un usuario interactúa activamente con aplicaciones que requieren respuestas rápidas (procesos I/O-bound), las colas `p0` y `p1` tendrán mucha actividad, y los procesos CPU-bound en `p2` y `p3` tendrán que esperar.
* Si el usuario deja de interactuar y las aplicaciones pasan a un estado de espera, las colas `p0` y `p1` se vaciarán, permitiendo que los procesos CPU-bound en `p2` y `p3` se ejecuten.

1. **Ciclos de Actividad y Espera** :

* Este comportamiento es muy común en sistemas interactivos. Los procesos I/O-bound tienden a ser intermitentes (se ejecutan y luego esperan por I/O), mientras que los procesos CPU-bound pueden ejecutarse de manera más continua.
* Esto crea un flujo natural donde los procesos CPU-bound tienen oportunidades de ejecutarse cuando los procesos interactivos están esperando, y viceversa.

---

### ¿Es un Problema que los Procesos Casi Nunca Lleguen a `p2`?

1. **No necesariamente** . El hecho de que un proceso en `p1` no se degrade frecuentemente a `p2` no es un problema si el sistema está funcionando eficientemente y manejando las prioridades correctamente.
2. **Es una Característica, No un Problema** : Si un proceso en `p1` tiene muchas oportunidades de ejecutarse porque las colas de mayor prioridad se vacían regularmente, significa que el sistema está proporcionando suficiente tiempo de CPU para los procesos sin que haya necesidad de degradarlos más.
3. **Envejecimiento como Seguro** : El mecanismo de aging es un "seguro" para garantizar que los procesos en `p2` y `p3` no se queden esperando indefinidamente. Aunque estos procesos pueden tener que esperar más tiempo, el aging asegura que eventualmente serán ejecutados.

---

### Resumen

* **El sistema es dinámico** y se adapta a las condiciones de carga actuales. La importancia de las colas de prioridad cambia según el contexto.
* **Los procesos en `p1` podrían no degradarse con frecuencia a `p2`** , pero eso no es necesariamente un problema. Es simplemente una consecuencia del scheduler priorizando adecuadamente los procesos interactivos o I/O-bound.
* **El mecanismo de aging** asegura que los procesos en colas de baja prioridad no sean ignorados para siempre, proporcionando un balance entre responsividad y equidad.

En conclusión, este enfoque dinámico hace que el sistema sea eficiente en diferentes contextos y garantiza un balance adecuado entre la ejecución de procesos interactivos y CPU-bound. ¿Te gustaría explorar algún detalle específico o tienes más preguntas sobre cómo funciona este sistema? ¡Estoy aquí para ayudar!

asdasd

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
