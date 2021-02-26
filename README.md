# ezbus_runner
eZBus runner

# Compile
```
git submodule init
git submodule update
make
```
Two run two nodes with id's 1 and 2, do something like:

```
./ezbus_runner_1 -i 1 2> /dev/null &

./ezbus_runner_1 -i 2
```
...and so on...
