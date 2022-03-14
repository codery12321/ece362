# The nature of documentation
ECE 362 is not a class about microprocessors, or microcontrollers, or digital interfacing, or assembly language programming. ...And you find this out now, just when you were starting to get used to the whole process?

ECE 362 is *really* just a course in learning how to read, comprehend, and use **documentation**. Now that we've clarified that, it's time for...

> ## The first rule of documentation:
> All documentation is incorrect.

It may not be completely incorrect. It may be only slightly incorrect. You may even think that a piece of documentation is perfect and without flaw, but, if you look long enough and carefully enough, you'll find something wrong. It may be so innocuous that no one cares about it. If an error in a piece of documentation is so obscure that no one notices it, and it has no consequences, then it's good enough. This brings us to the second rule of documentation...

The second rule of documentation:
Bad documentation is better than no documentation at all.

Anytime you read instructions or an explanation of something, and it clearly has errors, before you get angry about it, consider how well you'd be getting by if you didn't have anything at all.

The common situation that you will experience time and again in your future career as a computer engineer is when you are reading a document that obviously has been translated from another language. Perhaps you only find trivial things such as a subject heading that says LNITIALIATION SWUENCE instead of INITIALIZATION SEQUENCE. It's not so bad. You can guess what it means.

Many stories are told of struggles to read translated documents, but consider how bad it would be if you didn't have that translation. How well do you read Chinese? How well do you read Korean? How about Japanese? Thai? Kannada? As industrial globalization continues, you can expect to see others. You might hope to be able to run a document through a software translation tool, but how long will it be before such a thing will be better than a translation by a human?

Once, a team of students was working on a mini-project and trying to learn how to use a hardware device. I was trying to explain how the device should work by stepping through the documentation, which was terrible (by their standards) because it had been translated. Then one of the students, who was Chinese, decided to go about finding the original Chinese documentation. I left them go at that point while they went about looking for the original documentation. Some time later, after they managed to get their hardware to work, I asked them if the Chinese documentation was helpful. They said, "It was worse than the English version."

This is not an indictment of Chinese writers. It's possible that this incident said far more about the readers than about the documentation.

Sometimes documentation errors go beyond the basic difficulty of reading mispellings, grammar errors, and clumsy prose. Often, documentation will contain fundamental errors for specifications. For instance, there may be two conflicting sentences like this:

If the serial configuration value 01001011 is issued to the Bake-o-tron 3000, it will commence automatic cupcake production.

and
If the serial configuration value 01001011 is ever accidentally sent to the Bake-o-tron 3000, it will rapidly overheat and explode violently.

This documentation is actually extremely helpful in properly alerting an astute reader to the reality of the poor design philosophy of the product. Once again, it is important to consider how well one would be getting along without this documentation! ...subjected to the Faustian bargain of delicious cupcakes and fiery demise.

This is not a completely implausible situation. Devices are often produced that can be put into modes of operation that will cause them to break laws, interfere with other systems, and destroy themselves. It is up to the reader of the documentation to understand these situations before using them.

The larger truth is that even though the documentation is incorrect, it is clear that one of the values must be wrong, and that the selection of these values has consequences. Perhaps further reading will show a table of all serial configuration values that will confirm what each code does. Maybe there is some meaning to each individual bit of the code that indicates some conflict between certain bits that causes disaster. Such a discovery would provide greater insight into how the system actually works.

Eventually, you might reach the conclusion that the documentation would have been more helpful if it had only started with some fundamental element that was completely omitted. Once you reach the point that you feel you could do a better job of describing a system than its official documenter, then you can be sure that you understand it. Maybe you understand it better than you were supposed to...

The third rule of documentation:
All documentation should be read with the belief that it was intentionally written to deceive you.

Well, that's a pessimistic outlook, isn't it?

Often, documentation may be written properly, but before the device it describes had been finished. Subsequent changes to the system may not be reflected in the documentation. Other times, it is only written as an afterthought, because it had to be done, because someone told someone else that it had to be written. When documentation is written by people who really don't want to do so, you can start to understand why they may hope you don't understand it.

The fact is that you will read a great deal of documentation in your future career. It will be filled with mistakes. Your success will be based, to a great degree, on your ability to not only understand the documentation, but also to anticipate and overcome the inaccuracies. The best way to do that is maintain an extreme skepticism about the correctness of the documentation. For instance, read this table of values:

Value	Color
0	Black
1	Blue
2	Green
3	Yellow
4	Red
5	Violet
6	Cyan
7	White
As you read along, you should be silently be saying to yourself, "Lies! All lies!" (You can say it out loud if you want to, but that tends to worry coworkers and innocent bystanders.) That will prepare you to look critically at the table. When you notice that color 3 (Yellow) should probably be the bitwise binary OR of colors 1 and 2 (Blue and Green), you will not be surprised if a subsequent trial of the system shows that colors 1 and 4 in the table are exchanged. Then, you can say to yourself, "I knew it. You can't trick me."
The only true documentation for a system is its software source code and hardware schematics. Over time, you will develop a sense of how things generally work, and you may completely overlook errors in documentation. They won't even be visible to you because they don't make sense, and your brain will filter them out. Until you reach that point, it is good to maintain skepticism about everything you read until:

you see it repeated multiple times in the same document
you find everything in the document to be consistent
you've actually tried it yourself
Someday, I may write a note about the process of writing documentation. That's going to be an interesting read. The techniques I've mentioned when reading documentation also apply for when you're writing documentation.

Questions or comments about the course and/or the content of these webpages should be sent to the Course Webmaster. All the materials on this site are intended solely for the use of students enrolled in ECE 362 at the Purdue University West Lafayette Campus. Downloading, copying, or reproducing any of the copyrighted materials posted on this site (documents or videos) for anything other than educational purposes is forbidden.
