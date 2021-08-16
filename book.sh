rm -rf /tmp/book
cd book
git worktree add /tmp/book gh-pages
mdbook build
rm -rf /tmp/book/* .clang-format
cp -rp * /tmp/book/
cd /tmp/book
git add -A
git commit -m "update book"
git push origin gh-pages
cd -