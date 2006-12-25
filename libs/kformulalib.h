/**
 * @libdoc A formula editing and rendering library.
 *
 * Every formula is stored as a tree of element objects that more or less are
 * equivalent to the XML structure of the MathML they were loaded from.
 * The element objects are classes that derive all from the BasicElement class.
 * They all implement a special interface which is used to alter and navigate
 * through them.
 *
 * The element tree's highest element is an object of the class FormulaElement.
 * This object is a private member of the FormulaShape class which implements the
 * public interface of a flake shape.
 * The according flake tool is implemented in FormulaTool and contains an instance
 * of FormulaCursor which is a class that emulates the behaviour of a normal text
 * cursor for the formula. Through the FormulaCursor all editing is done and apart
 * from loading the formula tree's structure is never changed in a different way.
 *
 * 
 */
