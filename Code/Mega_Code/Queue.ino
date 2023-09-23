/*  Builds the priority queues for block placement based on 
 *  the location of the first randomly placed block. For consistency,
 *  if a restart is initiated, this block should be entered first
 *  to guarantee queue is built the same.
 */
void buildQueue(Block block, int pos) {
  int wheelChoice = buildWheelQueue(block, pos);

  buildFanQueue(block, pos, wheelChoice);
  buildBatteryQueue(block, pos);
}

/*  Builds the wheel queue based on the original block type and position.
 *  Will generate a tank wheel configuration.
 *  Returns which queue was chosen.
 */
int buildWheelQueue(Block block, int pos) {
  int queue1[] = {6, 2, 7, 1, 8, 0};
  int queue2[] = {5, 9, 4, 10, 3, 11};
  int queueSize = 6;
  int choice;

  // Check if the random block is on the side.
  // If it is and it's a wheel, use that side. Otherwise, use the other side.
  if (contains(queue1, queueSize, pos)) {
    if (block == WHEEL) {
      setQueue(wheelQueue, queue1, queueSize, pos);
      choice = 1;
    } else {
      setQueue(wheelQueue, queue2, queueSize, pos);
      choice = 2;
    }
  } else if (contains(queue2, queueSize, pos)) {
    if (block == WHEEL) {
      setQueue(wheelQueue, queue2, queueSize, pos);
      choice = 2;
    } else {
      setQueue(wheelQueue, queue1, queueSize, pos);
      choice = 1;
    }
  } else {
    // Default to first queue
    setQueue(wheelQueue, queue1, queueSize, pos);
    choice = 1;
  }

  if (debugMode) {
    debugPrint("Wheel queue consists of values: ");

    for (int i = 0; i < wheelQueue.size; i++) {
      debugPrint(wheelQueue.queue[i] + 48);
      debugPrint(' ');
    }
    debugPrint('\n');
  }

  return choice;
}

/*  Builds the fan queue based on the randomly placed block type and position, avoiding
 *  collision with the chosen wheel queue.
 */
void buildFanQueue(Block block, int pos, int wheelChoice) {
  int queue1[] = {0, 2, 6, 8};
  int queue2[] = {3, 5, 9, 11};
  int queue3[] = {12, 14, 16, 18};
  int queueSize = 4;
  int *queueChoice;

  // Check if the random block is in a queue spot.
  // If it's a fan, use that queue. Otherwise, if it's on side, use top. 
  if (contains(queue1, queueSize, pos)) {
    if (block == FAN) {
      queueChoice = queue1;
    } else {
      queueChoice = queue3;
    }
  } else if (contains(queue2, queueSize, pos)) {
    if (block == FAN) {
      queueChoice = queue2;
    } else {
      queueChoice = queue3;
    }
  } else if (contains(queue3, queueSize, pos)) {
    if (block == FAN) {
      queueChoice = queue3;
    } else if (wheelChoice == 1) {
      queueChoice = queue2;
    } else {
      queueChoice = queue1;
    }
  }
  // Since no blocking, use non-wheel side
  else if (wheelChoice == 1) {
    queueChoice = queue2;
  }
  else {
    queueChoice = queue1;
  }

  setQueue(fanQueue, queueChoice, queueSize, pos);

  if (debugMode) {
    debugPrint("Fan queue consists of values: ");

    for (int i = 0; i < fanQueue.size; i++) {
      debugPrint(fanQueue.queue[i] + 48);
      debugPrint(' ');
    }
    debugPrint('\n');
  }
}

/*  Builds the battery queue out of all of the remaining positions not captured by
 *  the wheel and fan queues.
 */
void buildBatteryQueue(Block block, int pos) {
  int queueSize = 10;
  int queue[queueSize];
  int index = 0;

  // Iterate through all 20 positions, checking if they're captured by the
  // wheel or fan queues. If not, add it to battery queue.
  for (int i = 0; i < 20; i++) {
    if (!contains(wheelQueue.queue, wheelQueue.size, i) && !contains(fanQueue.queue, fanQueue.size, i)) {
      queue[index] = i;
      index++;
    }
  }

  setQueue(batteryQueue, queue, queueSize, pos);

  if (debugMode) {
    debugPrint("Battery queue consists of values: ");

    for (int i = 0; i < batteryQueue.size; i++) {
      debugPrint(batteryQueue.queue[i] + 48);
      debugPrint(' ');
    }
    debugPrint('\n');
  }
}

/*  Sets the queue structure for one of the given block types to the queue information provided.
 *  If a block is already placed, swaps that to the front of the queue then advances the index.
 *  queue is the array to be used, size is the size of the array, and pos is the position being checked.
 */
void setQueue(PriorityQueue queueStruct, int queue[], int size, int pos) {
  queueStruct.size = size;
  queueStruct.index = 0;

  for (int i = 0; i < size; i++) {
    queueStruct.queue[i] = queue[i];
  }

  if (contains(queue, size, pos)) {
    // Get pos index
    int posIndex = getIndex(queue, size, pos);

    swapTo(queue, posIndex, 0);
    queueStruct.index++;
  }
}

/*  Updates the state of the queues with additional blocks that have been added, in case of a mid-competition restart.
 *  Swaps the new position to the front of the relevant queue and increases that queue's index by 1.
 */
void updateQueue(Block block, int pos) {
  struct PriorityQueue queueStruct;

  switch (block) {
    case WHEEL:
      queueStruct = wheelQueue;
      break;
    case FAN:
      queueStruct = fanQueue;
      break;
    default:
      queueStruct = batteryQueue;
      break;
  }

  if (contains(queueStruct.queue, queueStruct.size, pos)) {
    // Get pos index
    int posIndex = getIndex(queueStruct.queue, queueStruct.size, pos);

    swapTo(queueStruct.queue, posIndex, 0);
    queueStruct.index++;
  }

  if (debugMode) {
    char* str;

    switch (block) {
      case WHEEL:
        str = "wheel";
        break;
      case FAN:
        str = "fan";
        break;
      default:
        str = "battery";
        break;
    }

    debugPrint("Updating ");
    debugPrint(str);
    debugPrint(" queue with a new block at position ");
    debugPrintln(pos);

    debugPrintln("New queue state is: ");

    for (int i = 0; i < queueStruct.size; i++) {
      debugPrint(queueStruct.queue[i] + 48);
      debugPrint(" ");
    }
    debugPrint('\n');

    debugPrint("With the new index at: ");
    debugPrintln(queueStruct.index + 48);
  }  
}

/*  Gets the array index corresponding to the given value. If the value is not contained in the array,
 *  returns -1.
 */
int getIndex(int queue[], int size, int value) {
  for (int i = 0; i < size; i++) {
    if (queue[i] == value) return i;
  }

  return -1;
}

/*  Moves the value at the given index 'start' to the desired index 'end' by swapping it
 *  one index at a time.
 *  WARNING: If start or end is not in the array, will cause buffer overflow.
 */
void swapTo(int queue[], int start, int end) {
  int sign;
  if (start > end) sign = -1;
  else sign = 1;

  for (int i = start; i != end; i += sign) {
    swap(queue, i, i+sign);
  }
}

/*  Swaps the elements at the given array indexes.
 */
void swap(int queue[], int left, int right) {
  int temp = queue[left];

  queue[left] = queue[right];
  queue[right] = temp;
}

/*  Checks if the value is contained within the given queue.
 */
bool contains(int queue[], int size, int value) {
  for (int i = 0; i < size; i++) {
    if (queue[i] == value) return true;
  }

  return false;
}