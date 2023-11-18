bool _queueBuilt;

/*  Checks if the queue has been built. */
bool isQueueBuilt() {
  return _queueBuilt;
}

/*  Builds the priority queues for block placement based on 
 *  the location of the first randomly placed block. For consistency,
 *  if a restart is initiated, this block should be entered first
 *  to guarantee queue is built the same.
 */
void buildQueue(Block block, int pos) {
  // Exit if invalid placement location
  if (pos > 20) {
    return;
  }

  int wheelChoice = buildWheelQueue(block, pos);

  buildFanQueue(block, pos, wheelChoice);
  buildBatteryQueue(block, pos);

  _queueBuilt = true;
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
}

/*  Sets the queue structure for one of the given block types to the queue information provided.
 *  If a block is already placed, swaps that to the front of the queue then advances the index.
 *  Queue is the array to be used, size is the size of the array, and pos is the position being checked.
 */
void setQueue(PriorityQueue &queueStruct, int queue[], int size, int pos) {
  queueStruct.size = size;
  queueStruct.index = 0;

  if (contains(queue, size, pos)) {
    // Get pos index
    int posIndex = getIndex(queue, size, pos);

    swapTo(queue, posIndex, 0);
    queueStruct.index++;
  }

  for (int i = 0; i < size; i++) {
    queueStruct.queue[i] = queue[i];
  }
}

/*  Updates the state of the queues with additional blocks that have been added, in case of a mid-competition restart.
 *  Swaps the new position to the front of the relevant queue and increases that queue's index by 1.
 */
void updateQueue(int pos) {
  // Check wheel queue
  if (contains(wheelQueue.queue, wheelQueue.size, pos)) {
    // Get pos index
    int posIndex = getIndex(wheelQueue.queue, wheelQueue.size, pos);

    swapTo(wheelQueue.queue, posIndex, 0);
    wheelQueue.index++;
  }
  // Check fan queue
  else if (contains(fanQueue.queue, fanQueue.size, pos)) {
    // Get pos index
    int posIndex = getIndex(fanQueue.queue, fanQueue.size, pos);

    swapTo(fanQueue.queue, posIndex, 0);
    fanQueue.index++;
  } 
  // Check battery queue
  else if (contains(batteryQueue.queue, batteryQueue.size, pos)) {
    // Get pos index
    int posIndex = getIndex(batteryQueue.queue, batteryQueue.size, pos);

    swapTo(batteryQueue.queue, posIndex, 0);
    batteryQueue.index++;
  }
}

/*  Gets the array index corresponding to the given value. If the value is not contained in the array,
 *  returns -1. */
int getIndex(int queue[], int size, int value) {
  for (int i = 0; i < size; i++) {
    if (queue[i] == value) return i;
  }

  return -1;
}

/*  Moves the value at the given index 'start' to the desired index 'end' by swapping it
 *  one index at a time. Will shift all preceding values one index to the right.
 *  WARNING: Does not check array bounds. If start or end is not within the array bounds, this method will cause a buffer overflow.
 */
void swapTo(int queue[], int start, int end) {
  int sign;
  if (start > end) sign = -1;
  else sign = 1;

  for (int i = start; i != end; i += sign) {
    swap(queue, i, i+sign);
  }
}

/*  Swaps the elements at the given array indexes. */
void swap(int queue[], int left, int right) {
  int temp = queue[left];

  queue[left] = queue[right];
  queue[right] = temp;
}

/*  Returns true if the int value is contained within the given array. */
bool contains(int queue[], int size, int value) {
  for (int i = 0; i < size; i++) {
    if (queue[i] == value) return true;
  }

  return false;
}

/*  Gets the next position in queue for the given block type and advances the queue index.
 *  If there are no available positions in queue for the given blocktype, this method will return -1.
 *  If block type is invalid for this competition, this method will return -1.
 */
int getNextPosition(Block block) {
  switch (block) {
    case WHEEL: {
      return getNextPosition(wheelQueue);
    } break;
    case FAN: {
      return getNextPosition(fanQueue);
    } break;
    case BATTERY: {
      return getNextPosition(batteryQueue);
    } break;
    default: {
      return -1;
    }
  }
}

/*  Gets the next position in the given priority queue and advances its index.
 *  If there are no available remaining positions, returns -1. */
int getNextPosition(PriorityQueue &queueStruct) {
  if (queueStruct.index >= queueStruct.size) {
    return -1;
  }

  return queueStruct.queue[queueStruct.index++];
}

/*  Returns true if all block placement queues have been exhausted. */
bool exhausted() {
  return wheelQueue.index >= wheelQueue.size &&
    fanQueue.index >= fanQueue.size &&
    batteryQueue.index >= batteryQueue.size;
}

/*  Resets all queues to default status. */
void resetQueues() {
  wheelQueue.index = 0;
  fanQueue.index = 0;
  batteryQueue.index = 0;

  for (int i = 0; i < wheelQueue.size; i++) {
    wheelQueue.queue[i] = 0;
  }
  for (int i = 0; i < fanQueue.size; i++) {
    fanQueue.queue[i] = 0;
  }
  for (int i = 0; i < batteryQueue.size; i++) {
    batteryQueue.queue[i] = 0;
  }

  _queueBuilt = false;
}