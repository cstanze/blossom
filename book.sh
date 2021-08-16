cd book
mdbook build
cd ..
cd book/book
git add -A
git commit -m "update book"
git push origin gh-pages
