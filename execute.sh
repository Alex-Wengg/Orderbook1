echo "Working ? ."
echo "Working ? ."

# Define the output executable name
EXECUTABLE="ExampleExecutable"

# Clean up previous builds
rm -f *.o $EXECUTABLE
echo "Working ? ."

# Compile the C++ source files into object files
g++ -c Order.cpp
g++ -c OrderList.cpp
g++ -c OrderTree.cpp
g++ -c Example.cpp

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

# Link the object files into an executable
g++ Order.o OrderList.o OrderTree.o Example.o -o $EXECUTABLE

# Check if linking was successful
if [ $? -ne 0 ]; then
    echo "Linking failed."
    exit 1
fi
# Run the executable
./$EXECUTABLE
