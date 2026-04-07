#!/usr/bin/env python
#
from DistUtilsExtra.command import build_i18n as build_i18n_orig
import os
import os.path

class build_i18n(build_i18n_orig.build_i18n):
    user_options = build_i18n_orig.build_i18n.user_options + \
        [('xml_files_no_trans=', None, '.xml.in files which should be '
                                       'left with the unlocalised '
                                       'texts')]

    def initialize_options(self):
        build_i18n_orig.build_i18n.initialize_options(self)
        self.xml_files_no_trans = []

    def run(self):
        build_i18n_orig.build_i18n.run(self)
        data_files = self.distribution.data_files

        # merge .in with translation
        try:
            file_set = eval(self.xml_files_no_trans)
        except:
            return
        for (target, files) in file_set:
            build_target = os.path.join("build", target)
            if not os.path.exists(build_target): 
                os.makedirs(build_target)
            files_merged = []
            for file in files:
                if file.endswith(".in"):
                    file_merged = os.path.basename(file[:-3])
                else:
                    file_merged = os.path.basename(file)
                file_merged = os.path.join(build_target, file_merged)
                cmd = ["intltool-merge", "-x", "-u", "--no-translations",
                       file, file_merged]
                self.spawn(cmd)
                files_merged.append(file_merged)
            data_files.append((target, files_merged))
