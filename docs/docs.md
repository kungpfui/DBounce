
Documentation
=============


Debounce Duration
-----------------
```c++
// debounce setup
DBounce<uint8_t, __DBNC_N(buttons), 25, 0> debounce(buttons);

// debounceMS = 25  //ms
// lockoutMS = 0    // zero results in no lock-out
```

The behaviour with debounce duration only is a delay of both edges by *debounceMS*. During this time the logic level has to be stable.

- Channel 1: input signal
- Channel 2: debounced input signal

![debounce only](debounce_only_std.png)

Glitches which are shorter than *debounceMS* are not recoginzed.

![glitch with debounce only](debounce_only_glitch.png)


Lock-out Duration
-----------------
```c++
// lock-out setup
DBounce<uint8_t, __DBNC_N(buttons), 0, 25> debounce(buttons);

// debounceMS = 0  // 0 means: no debounce
// lockoutMS = 25  // ms
```

The behaviour with lock-only duration only looks rather transparent.

![lock-out only setup](lockout_only_std.png)


Short glitches are recoginzed but the resulting signal has a duration of at least *lockoutMS*.

![glitch wuith lock-out only setup](lockout_only_glitch.png)


First deteced change counts. There is some processing delay which depends on CPU speed and number of handled inputs.

![bouncing with lock-out only setup](lockout_only_bounce.png)


Combined debounce and lock-out
-----------------------------
```c++
// combined setup
DBounce<uint8_t, __DBNC_N(buttons), 25, 25> debounce(buttons);

// debounceMS = 25  // ms
// lockoutMS = 25   // ms
```
Get the best out of both world. The combination allows fast reactions without unexpected "button presses" from distortion which can occure with lock-out only setups. Normally the debounce signal looks like a debounce-only signal...

![combined setup](combined_std.png)

... but short signals are prolonged to a least *debounceMS* + *lockoutMS*.

![combined setup with short signal](combined_short.png)


**Experience:**
An explicit real button press needs more than 50ms. That's a real life debounce timeout used in million of electronic devices which were developed by me for crappy wall and small tactile switches. Well, somebody would say that the response time is not good with 50ms but good response needs to be in balance with good robustness (normally against electric distortions).
