
# How to use git for group projects (ft_irc)

Is allways best practice to check the current branch before doing anything regarding git, in order to do 
that we need the following command :

---
```javascript
1.git branch -a                                //green represents the current branch
```
---


How do we combine two different branches
-

1.Pushing on the desired branch (the one you work on):
```javascript
1.git add file.ex
2.git commit -m "message"
3.git push
```
2.Switching to the Master branch by running:
```javascript
git switch branch_name
or 
git checkout branch_name                //Only if the current branch has been pushed, else it delets everything
```
3.Before merging it's very important to PULL first, to ensure that no remote changes!!!(That's how merge conflicts begin)!!! were done to the Master branch:
```javascript
git pull
```
4.Now it's time for the merge, since there might be conflicts (Allways check, the green screen represents current branch commit, and blue for the upcoming commit).  Before accepting the changes make sure everythin is fine:
```javascript
git merge desired_branch
```

---
How to recover
-
So in case the ,merge went wrong, and the Master Branch lost important work, you can allways go back to the oldest current comit by running:
```javascript
git log 
```

Now you should see the latest commits, you will look for the latest commit for the Master branch (HEAD). Copy the yellow commit number , and run the following command:

```javascript
git checkout YELLOW_COMMIT_NUMBER
```

---

How to get work from main branch to your desired branch
-

1. Switch to the main branch, and PULL (for good practice), than switch back to the desired branch. 
2. It's now time for the final step, which is just running the simple comand:

```javascript
git merge origin/main                                       
```


##FEEL FREE TO ADD MORE !
---
