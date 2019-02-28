# Priority queue system
# Author: Douglas Canevarollo

# The priority queue uses the max heap structure to always maintain the
# most important key as the first element of the queue or the root of the heap tree.
# Thus, the time complexity to retrieve data from the priority queue is O(1).
# This data structure can be used in the operational system's processes management, for instance.

from os import system, name  # Allow us to control the operational system's terminal.
from time import sleep  # To stop the current thread to we see the information on the terminal.


# Prints the menu with the commands tutorial on the screen.
def menu():
    clear()
    print("\nCommands:\n")
    print("i <key> : Inserts value <key> on the queue.")
    print("k <key> <new> : Increases the value of the 'key' to 'new value'.")
    print("m : Returns the top priority level key.")
    print("e : Delete and returns the top priority level key.")
    print("q : Quit the application.", end='\n\n')


# Separates the command line to return then to our control variables.
def get_command(read_op="", read_key=0, read_value=0):
    commands = input("> ").split()

    read_op = str(commands[0])

    if len(commands) > 1:
        read_key = int(commands[1])

    if len(commands) > 2:
        read_value = int(commands[2])

    return read_op, read_key, read_value


# Clears the terminal screen.
def clear():
    # For Windows.
    if name == 'nt':
        _ = system("cls")
    # For Mac and Linux (here, o.s. name is 'posix').
    else:
        _ = system("clear")


class MaxHeap:
    def __init__(self, root=None):
        self.queue = []  # Queue of records.
        self.root = root  # To easily extract the highest priority level.

    def make_heap(self):
        if len(self.queue) > 1:
            # Does nothing if the heap just have one element.
            # Even if it has two elements the heapify can't be called yet, so
            # we sort the queue decreasing manually.
            if len(self.queue) == 2:
                self.queue.sort(reverse=True)
            # Otherwise the heap structure can be built.
            else:
                for i in range(len(self.queue), -1, -1):
                    self.heapify(i)

        self.root = self.queue[0]

    # The heapify() methods is applied on our tree in every insertion or removal from the queue.
    # Its takes the queue size and the start index i of the recursion as the parameters and their
    # objective is to always let the biggest value as the main root.
    def heapify(self, i):
        largest = i
        left = 2 * i + 1
        right = 2 * i + 2

        # Down here we found the largest value between the current node (root) and their children.

        # Checks if the left child exists and if it's greater than root
        if left < len(self.queue) and self.queue[left] > self.queue[largest]:
            largest = left

        # Analogue
        if right < len(self.queue) and self.queue[right] > self.queue[largest]:
            largest = right

        # If the largest value was changed the heap tree must be too.
        if largest != i:
            self.queue[i], self.queue[largest] = self.queue[largest], self.queue[i]  # Swap the elements.
            self.heapify(largest)  # Recursively heapify the new node.

    # Inserts the element 'key' in the queue.
    def insert(self, key):
        self.queue.append(key)
        self.make_heap()

        print("\nKey successfully inserted!")
        sleep(1.5)

    # Returns the element with the biggest priority value.
    def max(self):
        # As I told, cause we using a max heap structure the biggest value is in the root.
        if self.root is not None:
            return self.root

    # Delete and returns the element with the biggest priority value.
    def extract_max(self):
        if self.root is not None:
            key = self.root

            self.queue.pop(0)
            self.make_heap()

            return key

    # Increases the value of 'key' to 'new_value'.
    def increase_key(self, key, new_value):
        if new_value <= key:
            raise ValueError("\nThe new value must be biggest than current value.")

        if self.root is not None:
            self.queue[self.queue.index(key)] = new_value
            self.make_heap()

            print("\nThe value of key %i was changed to %i." % (key, new_value))
            sleep(3)


# Our main data structure is the priority queue and its has their MaxHeap object
class PriorityQueue:
    def __init__(self):
        self.max_heap = MaxHeap()

    def print_queue(self):
        if self.max_heap.root is not None:
            print("Current priority queue\n>", end=' ')
            for key in self.max_heap.queue:
                print(key, end=' ')
            print("\n")
        else:
            print("Empty tree.\n")


# Driver code.
if __name__ == "__main__":
    clear()
    leave = False
    priority_queue = PriorityQueue()

    while not leave:
        try:
            menu()
            priority_queue.print_queue()
            # Operation, key and value read.
            op, k, v = get_command()

            if op is "i":
                priority_queue.max_heap.insert(k)
            elif op is "m":
                temp = priority_queue.max_heap.max()
                if temp is not None:
                    print("> %i" % temp)
                    sleep(3)
                else:
                    continue
            elif op is "e":
                temp = priority_queue.max_heap.extract_max()
                if temp is not None:
                    print("> %i" % temp)
                    sleep(3)
                else:
                    continue
            elif op is "k":
                priority_queue.max_heap.increase_key(k, v)
            elif op is "q":
                leave = True
        except ValueError as error:
            print(error.args)
            sleep(3)

    exit(0)
