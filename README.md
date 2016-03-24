# ofxThreadPool

easy thread pool with lambda

## Dependencies

* oF 0.9.0~

## API

### ofxThreadPool<Material, Result>

* Material: raw data type
* Result: processed data type

### ofxThreadPool<Material, Result>::ThreadProcess

`using ThreadProcess = std::function<Result(Material &)>;`

### ofxThreadPool<Material, Result>::CompleteCallback

`using CompleteCallback = std::function<void(Result &&)>;`

### void setup(ThreadProcess tp, CompleteCallback cc, std::size_t thread_num = 8)

* `tp` is called background.
* `cc` is called before `ofApp::update`

### void push(Material &&m)

0.9.3~, because ~0.9.2 causes crash.

### void push(const Material &m)

### void start()
### void stop()

start/stop threads

### void addThread()
### void removeThread()

add/remove thread

### bool isRunning() const;

is thread running

### std::size_t threadNum() const;

current thread num

### std::size_t processedPerFrame() const;

### float processedPerThread() const;

### std::size_t queuedNum() const;

number of queued material wait process

## Update history

### 2016/03/24 ver 0.0.1 release

* initial

## License

MIT License.

## Author

* ISHII 2bit [bufferRenaiss co., ltd.]
* ishii[at]buffer-renaiss.com

## At the last

Please create a new issue if there is a problem.
And please throw a pull request if you have a cool idea!!
