#!/bin/python3
import pypandoc

pandoc_formats = {'bib': ['bibtex', 'biblatex'], 'creole': ['creole'], 'json': ['csljson', 'json'], 'csv': ['csv'], 'tsv': ['tsv'], 'xml': ['docbook', 'endnotexml', 'jats', 'docbook5', 'jats_archiving', 'jats_articleauthoring', 'jats_publishing', 'tei'], 'docx': ['docx'], 'dokuwiki': ['dokuwiki'], 'epub': ['epub', 'epub3', 'epub2'], 'fb2': ['fb2'], 'md': ['markdown', 'gfm', 'commonmark', 'commonmark_x', 'markdown_github', 'markdown_mmd', 'markdown_phpextra', 'markdown_strict', 'markua'], 'haddock': ['haddock'], 'html': ['html', 'chunkedhtml', 'html5', 'html4', 'slideous', 'slidy', 'dzslides', 'revealjs', 's5'], 'ipynb': ['ipynb'], 'jira': ['jira'], 'tex': ['latex', 'beamer'], 'mediawiki': ['mediawiki'], 'man': ['man'], 'muse': ['muse'], 'native': ['native'], 'odt': ['odt', 'opendocument'], 'opml': ['opml'], 'org': ['org'], 'ris': ['ris'], 'rtf': ['rtf'], 'rst': ['rst'], 't2t': ['t2t'], 'textile': ['textile'], 'tikiwiki': ['tikiwiki'], 'twiki': ['twiki'], 'typst': ['typst'], 'vimwiki': ['vimwiki'], 'adoc': ['asciidoc', 'asciidoctor'], 'txt': ['plain', 'asciidoc_legacy'], 'ctx': ['context'], 'icml': ['icml'], 'ms': ['ms'], 'pdf': ['pdf'], 'pptx': ['pptx'], 'texinfo': ['texinfo'], 'xwiki': ['xwiki'], 'zim': ['zimwiki']}

def convert(input_file_path, output_file_path):
    input_file_ext = input_file_path.split('.')[-1]
    output_file_ext = output_file_path.split('.')[-1]
    input_format = pandoc_formats.get(input_file_ext)
    output_format = pandoc_formats.get(output_file_ext)

    for in_form_iterate in input_format:
        for out_form_iterate in output_format:
            try:
                pypandoc.convert_file(input_file_path, out_form_iterate, format=in_form_iterate, outputfile=output_file_path)
                print(f"File converted from {in_form_iterate} to {out_form_iterate} successfully.")
                return True
            except Exception as e:
                last_exception = e
    print(f"An unexpected error occured: {last_exception}")
    return False
