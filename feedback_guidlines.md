#on submiting feature requests, bug reports, and patches



# Feature requests, bug reports, and patches #
I deeply value the feedback that I've gotten so far. It has been invaluable to me in finding problems and mapping out my plans for euclid. So a heartfelt "thank you" to all the testers and users who have taken time to help me improve euclid.

But here are some general points I'd ask you to consider before giving feedback.

The following are not directed to any individual: Although I use the 2nd person liberally, **do not take these comments personally**.

## Feature requests ##
Here are some general things to consider  before submitting a feature request:

First off, euclid-wm is meant to be minimalist; I refuse to succumb to feature creep. If a feature is genuinely going to save a lot of time, or make euclid capable of handling something it can't otherwise handle, I'm open to implementing it, but before I add a feature I want to know that there is a real need for it. This the first reason that euclid doesn't have tagging: tagging seems to me like something that is very rarely useful (although it is cool), and the benefit I see seems to me to be far outweighed by the two following considerations.

Second, I want euclid's code to be maintainable. In general I want to keep the code as comprehensible as possible without sacrificing important functions.

Third, and most importantly, I do not want to multiply keybindings without need. I want euclid's behavior well-defined, predictable, and easy to memorize. Adding 8 new keybindings to accomodate some rarely used feature is not desirable. This is the primary reason euclid doesn't have a floating layer (but the above considerations, and some others play a real role).

As to my estimation of whether these considerations ballance out, I'm open to hearing arguements in support of features I don't think are worth the cost. I'm perfectly willing to admit that I might have missed something, or that my vision of how euclid works out in real life isn't perfect.

So if you are approaching me with a feature request be ready to explain in detail why the feature is necessary. If I don't think a feature fits into euclid, I'll say so, and I'll probably list some reasons. Don't take it personally. When you asked for the feature you forced me to make a decision (whether to implement it or not), in explaining my reason I'm justifying my decision rather than just saying "no" without an explanation.

And please describe the feature in detail and how the desired feature differs from current behavior. Really spell it out.

Also, I'm much more receptive to feature requests that are phrased as "I'd really like to be able to . . ." or "I think it could be useful to be able to . . ." or "I'd switch to euclid full-time if I could . . ." or "do you have plans to implement . . ." as oposed to "euclid needs to  . . ." or what's worse, "you need to . . . " Does it? Do I? (Admittedly sometimes these are absolutely right; Sometimes I have made a mistake or just been lazy, but that is generally more applicable to bug reports than feature requests.)

Of course I'd really rather hear "I'd like to implement x. Where should I start?"

## Bug reports ##

I really appreciate bug reports: I want euclid to be stable and predictable. But please be as descriptive as possible. General "euclid doesn't act right when I <do some common activity>" doesn't do much to help me. I need as many details as you can give before I can even start tacking down--let alone fixing--the issue. This means steps (from a clean start) to reproduce the bahavior if it is possible.

And the bug may have already been corrected. So either update to the newest rev, or try to give me an idea of which revision you have installed (even if that is just telling me about what day you installed it and how you installed it, the newest versions of euclid write a VERSION file in /usr/share/euclid-wm).

If you are reporting a bug, please consider filing it under the "issues" tab. This draws visibility to it for the sake of other curent users or potential users who might experience the same issue. I haven't always been good about this myself, but I would like to get it so that "issues" really is a complete list of all significant known issues allong with any information on work-arounds, debugging the problems, and fixing them. (This isn't really a big deal, just a preference.) Also when reporting issues, please enter separate issues for seperate bugs. This makes everything a lot easier.

Sending a precise description of the bug is the most important part of reporting a bug, _even if you think you have a fix for it_.

## Patches ##
Writing a patch is by far the fastest way to get something implemented in euclid (if you want something done **now**, you often have to do it yourself). Contrary to some people's opinion, I do have a life, and my list of priorities for euclid may not be the same as yours (so your must-have feature may be in the at-some-indefinite-point-in-the-future category on my list).

If you are thinking about writing a patch, for the sake of your own time it is probably wise to contact me before you have too much invested in it. This is because (a) I might not want the patch (b) I know the code pretty well, and could probably save you some time.

If you don't like my attitude, my plans for the project, or in general the direction I'm taking euclid: I'm sorry. But for your consolation the code is BSD licensed and available through anonymous svn, so you are free to fork at any time. This of course is not to say that I want it forked, but if it does get to the point where you say to yourself "euclid is perfect for me except for x, y, z, which I cannot live without and he has made it completely clear that he will never implement any of these features" then I'm not going to stand in the way of your having your perfect wm. That's the beauty of OSS.