# Part 1: Repository Setup
In this step, we will essentially be cloning the Micromouse_2026 Git repository into your Micromouse folder. If this sounds not too technically difficult, feel free to skim over this part, but if you feel like you may not be 100% certain or comfortable with doing so, please refer to this document.

Assuming you have met all of the pre-requisites, the first thing to do is to create a dedicated folder for all of your micromouse related-files. This is helpful to keep everything organized for you. A useful keyboard shortcut is **Windows + E**, and this automatically opens file explorer, which could be of use to you for this tutorial.

To make a new folder, you can simply right-click, locate the `New` button in the menu, hover your mouse over it, and locate the button `folder`, see figure 1.1 below for reference:

[figure 1 goes here]

###### Figure 1.1 - This is the menu you see when you right-click anywhere in file explorer. Your interface may look slightly different, but the concept is the same, right click and look for the “new folder” button. Also take notice the option “Open in Terminal” in this menu as well, as it will be quite useful in this tutorial

With that being said, make a folder and name it `Micromouse`. It would be helpful to pin this folder to the sidebar, so you do not have difficulty finding it later on. Navigate to the inside of the `Micromouse` folder, and as it is a newly created folder, it should be blank/empty.

Right click anywhere and look for the option **Open in Terminal** and select it. This should bring you to a screen which looks like the image in figure 1.2:

[figure 2 goes here]
###### Figure 1.2 - Note the “file path,” denoted  as `C:\Users\raksh\Downloads\Micromouse>`. This may vary depending on where you created the micromouse folder, as well as the username on your computer, but ultimately you should see Micromouse folder at the end of the file path

Once you get to this stage, you will want to clone the Micromouse_2026 repository into your Micromouse folder. To do this, go to the following link: https://github.com/RutgersMicromouse/Micromouse-2026/tree/release/jerrieee 

Then locate the green “code” button and click it, see figure 1.3:

[figure 1.3 goes here]

###### Figure 1.3 - The “code” button is circled in red, click it to see the “clone” URL

Upon clicking the “code” button, you will see a panel that looks like this:

[figure 4 goes here]

###### Figure 1.4 - Panel visible upon clicking the “clone” button
Notice the URL, https://github.com/RutgersMicromouse/Micromouse-2026.git, you will want this URL to be in your clipboard, so copy this URL.

Now, go back to the terminal you just created as seen in Figure 1.2. It’s completely fine if you closed it earlier, to open it again, simply navigate back to the Micromouse folder, right click, and select “Open in Terminal." Once you see the file path we saw earlier in step 1.2, then you can be assured that you are in the right terminal.

Once in the Micromouse terminal and having the URL in your keyboard, you will clone the repository by typing the following command:
git clone https://github.com/RutgersMicromouse/Micromouse-2026.git

Assuming everything goes correctly, this will create the Micromouse_2026 repository in your Micromouse folder. If you see something like “git is not a recognized command,” then you will need to install git. 

Once you see the Micromouse_2026 folder inside of your Micromouse folder, you will want to open the terminal in this folder and run the following command:
git submodule update --init --recursive

This command downloads additional repositories that the main project depends on (such as chip headers and libraries). Once this command is successfully run, you should now see additional folders populate inside the repository, and once you do, you will be ready to move on to Part 2.
