<h1>Cool, so you have this super sexy object now. How do turn it into an actual prototyping tool? The answer is software.&nbsp;</h1>


<p>After you've unboxed (<a href="https://hackaday.io/project/191238/instructions">and maybe assembled</a>) your Jumperless, we're going to need a way to talk to it.&nbsp;</p>


<p>Now, the Jumperless is very open and will accept netlists from any program with some basic formatting, if you hate Wokwi and want to use something else, <a href="https://hackaday.io/project/191238-jumperless/log/222353-the-code">this project log</a> should give you an idea of what format it accepts over serial so you can write your own Bridge to any software you like. But for now, I'm focusing on Wokwi because I think it's better to have solid support for one workflow than shitty support for a bunch.</p>


<h2>First, we need to <a href="https://github.com/Architeuthis-Flux/Jumperless/releases">download the App and latest firmware here</a>.</h2>

<h1>Updating The Firmware</h1>


<p>I ship these with the latest firmware installed, so if you just received your Jumperless in the mail, you <em>can</em>&nbsp;skip the firmware update. But I usually push minor fixes to that firmware.uf2 file every few days, so it's probably worth doing anyway.</p>


<h2></h2>

<p>On your Jumperless, there will be a little button on the opposite side of the USB port. This is the USB Boot button and works exactly the same as a Raspberry Pi Pico.&nbsp;&nbsp;</p>

<figure><img class="lazy" src="https://cdn.hackaday.io/images/6226751694021856865.jpeg"></figure>

<p>1. Hold the USB Boot button and plug the Jumperless into your computer.&nbsp;<br></p>


<figure><a href="https://cdn.hackaday.io/images/8163571691016297318.png"><img class="lazy" style="max-width: 550px; display: block; margin: 1em auto;" src="https://cdn.hackaday.io/images/8163571691016297318.png"></a></figure>


<figure><a href="https://cdn.hackaday.io/images/903071691016314522.png"><img class="lazy" style="max-width: 550px; display: block; margin: 1em auto;" src="https://cdn.hackaday.io/images/903071691016314522.png"></a></figure>


<p>2. A drive should pop up called RPI-RP2. Drag the firmware.uf2 file into it and it should reset and you're done!</p>

<figure><img class="lazy" style="max-width: 550px; display: block; margin: 1em auto;" src="https://cdn.hackaday.io/images/679841691016326746.png"></figure>


<h1>Installing the App</h1>


<p>I'm showing this on a fresh install of macOS, but Windows should be roughly the same.</p>


<p>1. On the <a href="https://github.com/Architeuthis-Flux/Jumperless/releases">releases page</a>, download JumperlessWokwiBridgeMacOS.zip (or the .exe for windows) and firmware.uf2<br></p>


<figure><img class="lazy" src="https://cdn.hackaday.io/images/3415041694018229195.png"></figure>


<figure><img class="lazy" src="https://cdn.hackaday.io/images/2473941694018244082.png"></figure>


<p>2. It should automatically unzip in your downloads folder. Note that this won't run correctly from anywhere but your Applications folder, so drag it there.</p>


<figure><img class="lazy" src="https://cdn.hackaday.io/images/5095191694018424265.png"></figure>


<p>3. Click it to try to run it. Unless you've set your security setting to allow apps from anywhere, it's going to complain.</p>


<figure><img class="lazy" src="https://cdn.hackaday.io/images/2097991694018716935.png"></figure>


<p>4. Click Cancel and go to&nbsp;&nbsp;Settings &gt; Privacy and Security &gt; scroll down and click Open Anyway</p>


<figure><img class="lazy" src="https://cdn.hackaday.io/images/9218181694018787634.png"></figure>


<p>5. It's going to complain again and just click Open</p>


<figure><img class="lazy" src="https://cdn.hackaday.io/images/5342061694018862259.png"></figure>


<p>Hot tip: run&nbsp;</p>


<pre class="hljs bash"><span class="hljs-attribute">sudo</span> spctl --master-disable</pre>


<p>in Terminal to give you another checkbox for Anywhere under Settings &gt; Privacy and Security &gt; Allow Apps From. And you won't have to go through this again for other shoddy software.</p>


<figure><img class="lazy" src="https://cdn.hackaday.io/images/2858041694019134869.png"></figure>


<p>6. If everything goes well, you should now be presented with a Terminal window like this</p>


<figure><img class="lazy" src="https://cdn.hackaday.io/images/4168321694019999967.png"></figure>


<p>Note that on MacOS, it should just autodetect and connect to the Jumperless if it's plugged in when you start the app, if not, just plug it in and type 'r' to rescan the ports until it finds it.&nbsp;</p>


<p>Okay, now the app is set up, what do we paste into there to connect to our Wokwi project?</p>


<h2>Getting Wokwi Set Up</h2>


<p>You'll need a stable link to your Wokwi project, and it will only make one when you're signed in and have saved your project to "My Projects". After that the links are saved in a text file in the JumperlessWokwiBridge app and you'll be able to select projects you've used before from a list.</p>


<ol><li>Go to&nbsp;<a href="http://wokwi.com/" rel="noopener nofollow ugc">wokwi.com</a>

  
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/f/7/f7de5df4763a0544d6d58ba4e7c642d29834174f.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.53.39 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.53.39 PM" width="517" height="334" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/f/7/f7de5df4763a0544d6d58ba4e7c642d29834174f_2_517x334.jpeg"></a></li><li>
  
  
  Click Sign Up/Sign In and enter your email (or use Google or Github)
  
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/0/9/0915516b74460d993958e8f77cabaee84639e4e9.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.54.14 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.54.14 PM" width="517" height="334" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/0/9/0915516b74460d993958e8f77cabaee84639e4e9_2_517x334.jpeg"><br></a></li><li>
  
  
  Click the link they email you to sign in

  
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/5/3/53656d9d6683efaf1855c9d85ad8d4615fa42558.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.54.36 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.54.36 PM" width="345" height="223" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/5/3/53656d9d6683efaf1855c9d85ad8d4615fa42558_2_345x223.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/2/1/2193bcb2985a76ee92f7778e0d0688ecfd38b4f8.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.54.57 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.54.57 PM" width="345" height="223" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/2/1/2193bcb2985a76ee92f7778e0d0688ecfd38b4f8_2_345x223.jpeg"><br></a></li><li>
  
  
  Click Go To Your Projects, then New Project &gt; Arduino Nano
  
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/6/1/6161defa9bef70beb379ff7f412c6af332021a1a.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.55.08 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.55.08 PM" width="517" height="334" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/6/1/6161defa9bef70beb379ff7f412c6af332021a1a_2_517x334.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/8/a/8a395eb334448eed46381e4c851af3f2b958498f.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.55.13 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.55.13 PM" width="517" height="334" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/8/a/8a395eb334448eed46381e4c851af3f2b958498f_2_517x334.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/7/4/74fcff58629b5e4f1bf82326e2efa67b4c0416b5.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.55.20 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.55.20 PM" width="517" height="334" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/7/4/74fcff58629b5e4f1bf82326e2efa67b4c0416b5_2_517x334.jpeg"><br></a></li><li>


  Click Save and give your project a name

  
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/1/6/167d0209c2ee709fbd6da4993713e2befd9e4737.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.55.38 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.55.38 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/1/6/167d0209c2ee709fbd6da4993713e2befd9e4737_2_690x446.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/5/c/5c162e2a23c07b7c87a8564d4a5c32694486cec6.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.55.47 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.55.47 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/5/c/5c162e2a23c07b7c87a8564d4a5c32694486cec6_2_690x446.jpeg">&nbsp;</a>

(note that the URL has now changed to a unique link)</li><li>



Click blue “+” (top center) to Add New Part and scroll down to Half Breadboard and click it



<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/2/c/2c90eccd78931ac6fa2997c4137cc69bb716853a.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.10.47 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.10.47 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/2/c/2c90eccd78931ac6fa2997c4137cc69bb716853a_2_690x446.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/d/1/d198fa024756f543638f9b0dfbf9da38b999a333.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.11.02 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.11.02 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/d/1/d198fa024756f543638f9b0dfbf9da38b999a333_2_690x446.jpeg">&nbsp;</a>&nbsp;

It will put it somewhere random, so click somewhere that’s not a hole to drag it.



<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/d/c/dc8fdb3a831a4fd99d442914d751f83d81740909.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.11.07 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.11.07 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/d/c/dc8fdb3a831a4fd99d442914d751f83d81740909_2_690x446.jpeg"><br></a></li><li>


Open the JumperlessWokwiBridge App and resize the windows so you can see both



<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/7/b/7b5648a44207646fc73631753f0d22a8dcc562e6.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.56.14 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.56.14 PM" width="517" height="334" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/7/b/7b5648a44207646fc73631753f0d22a8dcc562e6_2_517x334.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/f/a/fa07016f125e1c21367242d1235f9e2e6b2e2774.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.58.03 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.58.03 PM" width="517" height="334" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/f/a/fa07016f125e1c21367242d1235f9e2e6b2e2774_2_517x334.jpeg"></a>
(on mac it will autodetect the port if it’s plugged in, on windows you’ll need to select the COM port here first, if the Jumperless wasn’t connected when you opened the app, press ‘r’ the Enter to rescan the ports)</li><li>


Copy the URL of the Wokwi project you just made

<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/9/4/946bd5208ed741f7bfda106806dace0ceb88f7be.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.58.12 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.58.12 PM" width="517" height="334" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/9/4/946bd5208ed741f7bfda106806dace0ceb88f7be_2_517x334.jpeg"><br></a></li><li>


Paste that URL into the Bridge App window and press Enter


<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/0/b/0b9118e2f2cc86229ecced7e2c3ba6a510351f0f.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.58.29 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.58.29 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/0/b/0b9118e2f2cc86229ecced7e2c3ba6a510351f0f_2_690x446.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/5/e/5ec325a39c12a56e3923e7a2a2caf338dd325876.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.58.34 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.58.34 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/5/e/5ec325a39c12a56e3923e7a2a2caf338dd325876_2_690x446.jpeg"><br></a></li><li>


Name the project and hit Enter (it will save the link so next time you’ll only need to choose it from a list by entering the number)

<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/a/1/a1b0387f1576fd97676db5be5c6999a8ff0573d1.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.58.46 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.58.46 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/a/1/a1b0387f1576fd97676db5be5c6999a8ff0573d1_2_690x446.jpeg"><br></a></li><li>

Now draw some wires on Wokwi by clicking a hole on the breadboard and dragging the wire to somewhere else


<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/7/c/7c36aac46a4dd1225fa70a28d97e46cac2500a2c.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.59.10 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.59.10 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/7/c/7c36aac46a4dd1225fa70a28d97e46cac2500a2c_2_690x446.jpeg"><br></a></li><li>

As soon as you press Save, the changes you made to the Wokwi project should show up on the Jumperless and you should be ready to start prototyping like a future person

<u><u><u><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/6/c/6cc78068959d60cef8a56cf72dec050fa15e1761.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 2.59.22 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 2.59.22 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/6/c/6cc78068959d60cef8a56cf72dec050fa15e1761_2_690x446.jpeg"><br></a></u></u></u></li></ol>




<h1>Extra Tips for using Jumperless like a Pro</h1>

<p>Here are some non-steps but more general tips:</p>

<h2>Debug Flags</h2>


<ul><li>If your Bridge app’s terminal is full of a bunch of debugging stuff, you should probably turn the debug flags off until you want to see a particular thing. Sending that much data over serial can occasionally cause the serial drivers to trip over themselves. Here’s how to turn those off:</li></ul>


<ol><li>From the main Menu (which it defaults to unless you’re in a sub menu,) type ‘d’ then Enter to go to the “toggle debug flags” menu.
<strong>
  
  <a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/6/3/63b663060ae9d4ffe8250f82a9bd6f384800abd4.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.25.54 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.25.54 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/6/3/63b663060ae9d4ffe8250f82a9bd6f384800abd4_2_690x446.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/8/d/8d332af91ec27ae315766e9efe67f398dea22a24.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.26.06 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.26.06 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/8/d/8d332af91ec27ae315766e9efe67f398dea22a24_2_690x446.jpeg"><br></a></strong></li><li>Type ‘0’ then Enter to turn them all off. Then ‘m’ and Enter to return to the main Menu
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/2/d/2d075e84ee0aa0f7dfcb7ae78eaa51be55174462.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.26.27 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.26.27 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/2/d/2d075e84ee0aa0f7dfcb7ae78eaa51be55174462_2_690x446.jpeg"><br></a></li></ol>

<p><span></span></p>

<h2>App Menu</h2>


<ul><li>If you want to do things that pertain to the Jumperless Wokwi Bridge App itself, type “menu” and then Enter (Note: after the part where you enter the Wokwi project link and choose a port, all this output you’re seeing is from the Jumperless’s onboard RP2040, and the Bridge app is just acting like the Serial Monitor from Arduino)
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/b/0/b03e6cd288a89897c0492a57abe346bf2df1d645.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.38.56 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.38.56 PM" width="570" height="500" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/b/0/b03e6cd288a89897c0492a57abe346bf2df1d645_2_570x500.jpeg"><br></a></li></ul>

<h2>Using the DACs and ADCs</h2>

<ul><li>To use the <strong>DACs</strong>, you’ll need to add 2 potentiometers to your Wokwi project and connect the Signal (center) pin to where you want it connected on the board. DAC 0 0-5V will be pot1 and DAC 1 ±8V will be pot2.</li></ul>


<ol><li>Blue “+” to Add New Part &gt; scroll down to Potentiometer (the linear Potentiometer will work too if you prefer) and click it
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/3/2/326af61e54fdf4657b6dea575a52588c6ce09c2d.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.46.51 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.46.51 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/3/2/326af61e54fdf4657b6dea575a52588c6ce09c2d_2_690x446.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/d/a/dafc0bb942c604982eac8ab3965b72cc82c4f8a2.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.47.00 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.47.00 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/d/a/dafc0bb942c604982eac8ab3965b72cc82c4f8a2_2_690x446.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/9/8/98a673fc94d5a4ec9a21925a01113bc50472b956.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.47.10 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.47.10 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/9/8/98a673fc94d5a4ec9a21925a01113bc50472b956_2_690x446.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/b/6/b61c3eec7aa216cc115243d70ba8e8ee294cf44e.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.47.41 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.47.41 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/b/6/b61c3eec7aa216cc115243d70ba8e8ee294cf44e_2_690x446.jpeg"><br></a></li><li>If you want the brightness and hue to track the voltage for DAC 1 ±8V, you’ll need to connect ADC3 (±8V scaled) to the same pin as DAC 1. ADC 0-3 are mapped to a Logic Analyzer pins 0-3 in Wokwi, I haven’t come up with a good way display the readings and I’m open to suggestions for how to show them without making the serial output a total mess. But in the code, AnalogRead(26,27,28, or 29) will work as expected, scaled to 0-5V for ADC 0-2 and ±8V for ADC 3. Anyway, here’s how to connect them:
  
Add New Part &gt; Logic Analyzer (8 channels)

<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/1/3/133e22e70162d0ed49c3fc3c4f3f299d199cfa3d.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.59.02 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.59.02 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/1/3/133e22e70162d0ed49c3fc3c4f3f299d199cfa3d_2_690x446.jpeg"><br></a><a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/e/0/e0057d80fca6caa037e26b24933c8fb4ddbdebc0.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 3.59.57 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 3.59.57 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/e/0/e0057d80fca6caa037e26b24933c8fb4ddbdebc0_2_690x446.jpeg"></a>
  
  Connect D3 (ADC 3) to the same breadboard row as pot2:Sig (DAC 1) and the brightness will track the output.
  
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/7/4/74e03f17beaa2d85fc29ef7bc10a0765f82f4c6f.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 4.00.24 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 4.00.24 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/7/4/74e03f17beaa2d85fc29ef7bc10a0765f82f4c6f_2_690x446.jpeg">&nbsp;</a>
  
  (Note: the pins are numbered the same as the ADCs, so D0 = ADC0 (pin 26), D1 = ADC1 (pin 27), etc.)</li><li>To control the DACs, enter Wavegen from the main Menu by typing ‘w’
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/0/5/0556e12aae6a8dd2c722cdb1ecee382f3f0ceeae.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 4.05.12 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 4.05.12 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/0/5/0556e12aae6a8dd2c722cdb1ecee382f3f0ceeae_2_690x446.jpeg"><br></a></li><li>
  Enter 5 or 8 twice to turn on the 0-5V or the ±8V DAC. The first time you enter it, it won’t turn it on so you can change the settings before you start it to avoid frying things.
  
<a href="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/original/3X/7/6/7621ceaf1e765e9a296b1802d4ec7e15dc35e82a.jpeg" class="lightbox" title="Screenshot 2023-09-05 at 4.08.49 PM" rel="noopener nofollow ugc"><img alt="Screenshot 2023-09-05 at 4.08.49 PM" width="690" height="446" class="d-lazyload lazy" style="border-style: none; vertical-align: middle; outline: 0px; object-fit: cover; object-position: center top; transition: opacity 0.4s ease 0.75s; max-width: 690px; max-height: 500px;" src="https://tindie-discourse.s3.dualstack.us-west-1.amazonaws.com/optimized/3X/7/6/7621ceaf1e765e9a296b1802d4ec7e15dc35e82a_2_690x446.jpeg"><br></a></li><li>
When you type ‘a’ or ‘o’ to change the Amplitude or Offset respectively, it expects numbers in the format ‘3.0’. It will wait for you to enter a decimal and then a number.
![Screenshot 2023-09-05 at 4 11 58 PM](https://github.com/Architeuthis-Flux/Jumperless/assets/20519442/444aadd6-8896-4d3d-857e-41bca201c80d)





<p>I’ll add more tricks as I think of them. If you have any questions about how to use any particular thing, or suggestions for things to work on supporting, let me know.</p>

<p>You can email me at KevinC@ppucc.io or message me through any channel you prefer.</p>
