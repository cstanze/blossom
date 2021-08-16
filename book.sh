cd book
mdbook build
cd ..
git worktree add ./book/book gh-pages
cd book/book
git add -A
git commit -m "update book"
git push origin gh-pages
