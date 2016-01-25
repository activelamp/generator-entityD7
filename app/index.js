var generators = require('yeoman-generator'),
    yosay = require('yosay'),
    chalk = require('chalk'),
    glob = require('glob'),
    fs = require('fs-extra'),
    changeCase = require('change-case'),
    path = require('path'),
    replace = require('replace');

module.exports = generators.Base.extend({

  askForProjectName: function() {
    var done = this.async();

    // have Yeoman greet the user
    this.log(yosay("Hello, you're about to " + chalk.red("create a new Drupal 7 entity.") + "  First answer some simple questions."));

    var prompts = [{
      name: 'moduleMachineName',
      message: 'What is the short name for the module?',
      default: this.appname.replace(" ", "-")
    },
    {
      name: 'moduleName',
      message: 'Would you like to name the Entity?',
    },
    {
      name: 'moduleDescription',
      message: 'Describe what this module does.'
    }
    ];

    this.prompt(prompts, function(props) {
      this.moduleMachineName = props.moduleMachineName;
      this.moduleName = props.moduleName;
      this.moduleDescription = props.moduleDescription;

      done();
    }.bind(this));
  },

  writing: {
    app: function() {
      var self = this;
      var files = glob.sync(self.templatePath() + '/**/*');

      files.forEach(function(file) {
        if (fs.lstatSync(file).isDirectory()) {
          // Don't try to copy a directory, they get created automatically.
          return;
        }

        var fileName = file.replace(self.templatePath() + '/', '');
        var newFileName = fileName
          .replace(/custom_entity/g, self.moduleMachineName)
          .replace(/CustomEntity/g, changeCase.pascalCase(self.moduleName));

        var dir = path.dirname(newFileName);
        var baseName = path.basename(newFileName);
        var extension = path.extname(baseName);

        newFileName = dir ? dir + '/' + baseName : baseName;

        if (extension === '.png' || extension === '.jpg') {
          self.fs.copy(self.templatePath(fileName), self.destinationPath(newFileName));
        }
        else {
          var contents = self.fs.read(self.templatePath(fileName));
          var newContents = contents
            .replace(/Custom Entity/g, changeCase.titleCase(self.moduleName))
            .replace(/CustomEntity/g, changeCase.pascalCase(self.moduleName))
            .replace(/custom_entity/g, changeCase.snakeCase(self.moduleName))
            .replace(/MODULE_DESCRIPTION/g, self.moduleDescription);

          self.fs.write(newFileName, newContents);
        }

      });

    }
  }
  
});