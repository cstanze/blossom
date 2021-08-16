git worktree add /tmp/book gh-pages
rm -rf /tmp/book/*
cp -rp book/* /tmp/book/
cd /tmp/book && git add -A && git commit -m "update book on $(date) by ${USER}" && git push origin gh-pages
