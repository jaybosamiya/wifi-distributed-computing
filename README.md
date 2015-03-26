WiFi Distributed Computing
==========================

Lots of compute cycles in any general computer go to waste, idling. What if we could leverage these to do computation, without really specifying who should be doing what computation?

This software is a PoC (Proof of Concept) of this basic idea.

Do note that you need to have a wireless interface which supports "Monitor" mode to be able to use this software.

This software is meant for educational purpose only. The author will not be held liable for any harm that is related to the software.

Note: This software was created as an assignment/project as part of the Operating Systems course at IIT Roorkee.

Instructions for Compilation
----------------------------

Compiling the server and client follows the steps of standard CMake based build.

Basically, just run the following steps in the source directory:

```
mkdir bin
cd bin
cmake ..
make
```

This will create two executable files, `wifi-math-server` and `wifi-math-client`

Usage Syntax
------------

Running any of the executable files without parameters gives the syntax.

Legal
-----

If you intend to use this software, for any commercial usage, or as part of any research paper, please contact the author for appropriate licensing.

Outside of these conditions, the software lies under the [MIT License](http://jay.mit-license.org/2015).

Disclaimer
----------

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.