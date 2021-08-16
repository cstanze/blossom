cd book
mdbook build
cd ..
git worktree add ./book/book gh-pages
git add -A
git commit -m "update book"
git push origin gh-pages
